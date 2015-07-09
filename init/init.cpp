#include <semaphore.h>
#include <printk.h>
#include <stl/iterator.hh>
#include <ecma48.h>
#include <fork.h>
volatile semaphore_t sem = 1;
volatile semaphore_t children_finished = -1;
volatile int owner = 0; // this is to force owner change, to test semaphors, instead of looping a constant number of times

void monitor();
void test_stl();
void test_fork();

// the init process
void init_process()
{
	printk("in init now :)\n");
	void test_fs();
	test_fs();
	monitor();

#if 0
	test_stl();
	test_fork();
#endif
}

void test_stl() {
	// put vector in a block, to test deallocation in dtor
	aos::vector<int> v1{};
	v1.push_back( 1 );
	v1.push_back( 2 );
	v1.push_back( 3 );
	v1.push_back( 4 );
	v1.push_back( 5 );
	v1.push_back( 6 );
	v1.push_back( 7 );
	auto b = v1.begin(); printf("*begin() = %d\n", *b);
	auto d = v1.front(); printf("front() = %d\n", d);
	auto f = v1[3]; printf("[3] = %d\n", f);
	auto h = v1.size(); printf("size = %d\n", h);
	auto u = v1.end();

	printf("erasing begin():     ");
	auto afaf = v1.erase(b);
	if(afaf == v1.end()) printf("no\n");
	if(afaf == v1.begin()+1) printf("yes\n");
	for(auto i : v1)
		printf("%d, ", i);
	printf("\n");

	printf("insert 3 at begin(): ");
	v1.insert(v1.begin(), 3);
	for(auto i : v1)
		printf("%d, ", i);
	printf("\n");

	v1.erase(v1.begin());
	v1.erase(v1.begin());
	v1.erase(v1.begin());

	aos::vector<int> v3{}; v3.push_back(133); v3.push_back(144); v3.push_back(155);
	printf("insert range at begin()+3: ");
	v1.insert(v1.begin() + 3, aos::begin(v3), aos::end(v3));

	for(auto i : v1)
		printf("%d, ", i);
	printf("\n");

	aos::string<300> s1{"hi"};
	printf("s1: %s\n", s1.c_str());
	if(s1 == aos::string<3>{"hi"} && !(s1== aos::string<3>{"hI"})) { printf("string == ok\n"); }

	printf("insert bulk at begin()+3: ");
	v1.insert(v1.begin() + 3, 4, 111); // had to use SFINAE for this one...
	for(auto i : v1)
		printf("%d, ", i);
	printf("\n");
	// __asm__ __volatile__ ("xchg %bx,%bx\n"); // magic breakpoint for bochs internal debugger
	aos::vector<int> const v2{};
	printf("1 \n");auto c = v2.begin();
	printf("2 \n");auto e = v2.front(); // TODO: should be invalid
	//printf("3 \n");auto g = v2[3]; // invalid access sicne v2 is empty
	printf("4 \n");auto v = v2.end();
	// v2.insert(v2.begin(), 3); // cannot insert into a const vector

	// printf("testing vector ended. dtors (and shared_array deallocation) should run now\n");

	// printf("testing vector ended. now going into monitor\n");

}

void test_fork() {
	int x = 5*6;
	printk("Forking: \n");
	int proc = fork();
	printk(TRACE "this is process %i\n",proc);
	if(!proc) // FIXME: fork returns zero to parent ??? this is wrong.
	{
		wait(&sem);
		owner = 0;
		while(x--)
		{
			if(x%6==5)
			{
				signal(&sem);
				// test semaphor: at no cost break printing 'A' except every 6 A's printed
				while(owner == 0)
					;
				wait(&sem);
				owner = 0;
			}
			printk(REDB "A" NORMAL);
			for(int i = 0; i < 0xFFF; i++);
		}
		signal(&sem);

		// wait for all threads to finish
		wait(&children_finished);
		printf("\n");

		monitor();
	}
	else
	{
		proc = fork();

		if(!proc)
		{
			wait(&sem);
			owner = 1;
			while(x--)
			{
				if(x%6==5)
				{
					signal(&sem);
					while(owner == 1)
						;
					wait(&sem);
					owner = 1;
				}
				printk(GREENB "B" NORMAL);
				for(int i = 0; i < 0xFFF; i++);
			}
			signal(&sem);
			signal(&children_finished);
		}
		else
		{
			wait(&sem);
			owner = 2;
			while(x--)
			{
				if(x%6==5)
				{
					signal(&sem);
					while(owner == 2)
						;
					wait(&sem);
					owner = 2;
				}
				printk(BLUEB "C" NORMAL);
				for(int i = 0; i < 0xFFF; i++);
			}
			signal(&sem);
			signal(&children_finished);
		}
	}
}

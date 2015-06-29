#include <kb.h>
#include <console.h>
#include <lib.h>
#include <idt.h>


int shift=0; // shift pressed ?
int ctrl=0;
int alt=0;
int numlock=0;
int capslock=0;
unsigned char* currKB=KeyboardUs;
unsigned char KeyboardUs[128] =
{
	0          , VK_ESC      , '1'      , '2'       , '3'      , '4'    , '5'   , '6'     , '7'         , '8'         ,
	'9'        , '0'         , '-'      , '='       , '\b'     , '\t'   , 'q'   , 'w'     , 'e'         , 'r'         ,
	't'        , 'y'         , 'u'      , 'i'       , 'o'      , 'p'    , '['   , ']'     , '\n'        , VK_CTRL     ,
	'a'        , 's'         , 'd'      , 'f'       , 'g'      , 'h'    , 'j'   , 'k'     , 'l'         , ';'         ,
	'\''       , '`'         , VK_LSHIFT, '\\'      , 'z'      , 'x'    , 'c'   , 'v'     , 'b'         , 'n'         ,
	'm'        , ','         , '.'      , '/'       , VK_RSHIFT, '*'    , VK_ALT, ' '     , VK_CAPS_LOCK, VK_F1       ,
	VK_F2      , VK_F3       , VK_F4    , VK_F5     , VK_F6    , VK_F7  , VK_F8 , VK_F9   , VK_F10      , VK_NUM_LOCK ,
	VK_SCR_LOCK, VK_HOME     , VK_UP    , VK_PAGE_UP, '-'      , VK_LEFT, 0     , VK_RIGHT, '+'         , VK_END      ,
	VK_DOWN    , VK_PAGE_DOWN, VK_INSERT, VK_DELETE , 0        , 0      , 0     , VK_F11  ,	VK_F12      , 0           ,
};

unsigned char shiftKeyboardUs[128] =
{
	0          , VK_ESC      , '!'      , '@'       , '#'      , '$'    , '%'   , '^'     , '&'         , '*'         ,
	'('        , ')'         , '_'      , '+'       , '\b'     , 0      , 'Q'   , 'W'     , 'E'         , 'R'         ,
	'T'        , 'Y'         , 'U'      , 'I'       , 'O'      , 'P'    , '{'   , '}'     , '\n'        , VK_CTRL     ,
	'A'        , 'S'         , 'D'      , 'F'       , 'G'      , 'H'    , 'J'   , 'K'     , 'L'         , ':'         ,
	'"'        , '~'         , VK_LSHIFT, '|'       , 'Z'      , 'X'    , 'C'   , 'V'     , 'B'         , 'N'         ,
	'M'        , '<'         , '>'      , '?'       , VK_RSHIFT, '*'    , VK_ALT, ' '     , VK_CAPS_LOCK, VK_F1       ,
	VK_F2      , VK_F3       , VK_F4    , VK_F5     , VK_F6    , VK_F7  , VK_F8 , VK_F9   , VK_F10      , VK_NUM_LOCK ,
	VK_SCR_LOCK, VK_HOME     , VK_UP    , VK_PAGE_UP, '-'      , VK_LEFT, 0     , VK_RIGHT, '+'         , VK_END      ,
	VK_DOWN    , VK_PAGE_DOWN, VK_INSERT, VK_DELETE , 0        , 0      , 0     , VK_F11  ,	VK_F12      , 0           ,
};

#include <asm.h>
static char* input = nullptr;
static int cnt_read = 0;
volatile static int newline_read = 0; // boolean
volatile static int freeze = 0; // input exceeded buffer, freeze additions
void readline(char* buf, int max)
{
	input = buf;
	while(1)
	{
		ASM("pause\n"
		    "hlt"); // hlt waits for interrupts

		if(newline_read)
			break;
		if(cnt_read == max)
			freeze=1;
	}
	*input++=0;

	// reset
	input = nullptr;
	cnt_read = 0;
	newline_read = 0;
	freeze = 0;
}

void readline_handler(unsigned char c)
{ // TODO handle backspaces
	if(nullptr == input)
		/* discard */;
	else
	{
		putchar(c);
		if('\n' == c)
			newline_read = 1;
		else if(!freeze)
			*input++ = c;
		cnt_read ++;
	}
}

int keyboard_handler(struct interrupt_frame *);

// 8042 PS/2 controller
// By default, translation is enabled to scancode set 1 (XT).
// There are two controllers: on-board (the PS/2 controller itself), and in-keyboard.
namespace iodev {
 namespace ps2 {
   constexpr uint16_t REG_DATA = 0x60;
   constexpr uint16_t REG_STATUS = 0x64; // read
   constexpr uint16_t REG_CMD = 0x64; // write: send command to ps/2 controller itself (not the devices)

   struct status_byte {
     /*implicit*/
     status_byte(unsigned char data)
       : output_buffer_full{0}, input_buffer_full{0}, system_flag{0}, type{0}, __unused{0}, timeout_error{0}, parity_error{0}
       { *this = static_cast<status_byte>(data); }
     // least significat bit first:
     unsigned output_buffer_full:1; // "output" from the perspective of the ps/2 device. to us, that's input.
     unsigned input_buffer_full:1;
     unsigned system_flag:1;
     unsigned type:1; // data written to input buffer is : 0 -> data for ps/2 device, 1 -> data for ps/2 controller command
     unsigned __unused:2;
     unsigned timeout_error:1;
     unsigned parity_error:1;
   } __attribute__((packed));

   struct controller_configuration_byte {
     /*implicit*/
     controller_configuration_byte(unsigned char data)
       : first_ps2_port_interrupt_enabled{0}, second_ps2_port_interrupt_enabled{0},
         system_flag{0}, __zero{0}, first_ps2_port_clock_enabled{0},
         second_ps2_port_clock_enabled{0}, first_ps2_port_translation_enabled{0},
         __zero2{0}
       { *this = static_cast<controller_configuration_byte>(data); }
     operator unsigned char() const { return static_cast<unsigned char>(*this); }
     unsigned first_ps2_port_interrupt_enabled:1;
     unsigned second_ps2_port_interrupt_enabled:1;
     unsigned system_flag:1; // 0 -> your OS shouldn't be running , 1 -> system passed POST
     unsigned __zero:1;
     unsigned first_ps2_port_clock_enabled:1;
     unsigned second_ps2_port_clock_enabled:1;
     unsigned first_ps2_port_translation_enabled:1;
     unsigned __zero2:1;
   } __attribute__((packed));


   struct controller_output_port {
     /*implicit*/
     controller_output_port(unsigned char data)
       : system_reset{1}, A20_gate{1},
         second_ps2_port_clock{0}, second_ps2_port_data{0},
         output_buffer_full_with_byte_from_first_ps2_port{0},
         output_buffer_full_with_byte_from_second_ps2_port{0},
         first_ps2_port_clock{0}, first_ps2_port_data{0}
       { *this = static_cast<controller_output_port>(data); }
     operator unsigned char() const { return static_cast<unsigned char>(*this); }
     unsigned system_reset:1; // on output. must be 1.
     unsigned A20_gate:1; // on output
     unsigned second_ps2_port_clock:1;
     unsigned second_ps2_port_data:1;
     unsigned output_buffer_full_with_byte_from_first_ps2_port:1; //  (connected to IRQ1)
     unsigned output_buffer_full_with_byte_from_second_ps2_port:1; //  (connected to IRQ12)
     unsigned first_ps2_port_clock:1;
     unsigned first_ps2_port_data:1;
   } __attribute__((packed));

   status_byte read_status_byte() {
     return status_byte{ inportb(REG_STATUS) };
   }

   void wait_input_buffer_full() {
     while( read_status_byte().input_buffer_full )
       /* wait */;
   }
   void send_data(uint8_t data) {
     wait_input_buffer_full();
     outportb(REG_DATA, data);
   }
   void send_cmd(uint8_t cmd) {
     outportb(REG_CMD, cmd);
   }
   void send_cmd(uint8_t first_byte, uint8_t second_byte) {
     outportb(REG_CMD, first_byte);
     send_data(second_byte);
   }
   void wait_output_buffer_empty() {
     while( !read_status_byte().output_buffer_full )
       /* wait */;
   }
   uint8_t read_response() {
     return inportb(REG_DATA);
   }
   uint8_t sendrecv_cmd(uint8_t cmd) {
     outportb(REG_CMD, cmd);
     return read_response();
   }
   uint8_t sendrecv_cmd(uint8_t first_byte, uint8_t second_byte) {
     outportb(REG_CMD, first_byte);
     send_data(second_byte);
     return read_response();
   }

// PS/2 controller commands (not commands to devices, but to the controller itself)
   controller_configuration_byte read_controller_configuration_byte() {
     return controller_configuration_byte{ sendrecv_cmd(0x20) };
   }
   void write_controller_configuration_byte(controller_configuration_byte ccb) {
     send_cmd(0x60, ccb);
   }

   controller_output_port read_controller_output_port() {
     return controller_output_port {sendrecv_cmd(0xD0) };
   }
   void write_controller_output_port(controller_output_port cop) {
     wait_output_buffer_empty();
     send_cmd(0xD1, cop);
   }

   void disable_second_port() {
     send_cmd(0xA7);
   }
   void enable_second_port() {
     send_cmd(0xA8);
   }
   void disable_first_port() {
     send_cmd(0xAD);
   }
   void enable_first_port() {
     send_cmd(0xAE);
   }
   bool test_controller() {
     // returns true if test passed
     uint8_t status = sendrecv_cmd(0xAA);
     assert(status == 0x55 || status == 0xFC);
     return status == 0x55;
   }

   enum class port_test_t : uint8_t {
     test_passed = 0,
       clock_line_stuck_low, clock_line_stuck_high,
       data_line_stuck_low, data_line_stuck_high
       };

   port_test_t test_second_port() {
     return static_cast<port_test_t>(sendrecv_cmd(0xA9));
   }
   port_test_t test_first_port() {
     return static_cast<port_test_t>(sendrecv_cmd(0xAB));
   }

   void write_byte_to_first_ps2_port_output_buffer(uint8_t data) {
     // (makes it look like the byte written was received from the first PS/2 port)
     send_cmd(0xD2, data);
   }

   void write_byte_to_second_ps2_port_output_buffer(uint8_t data) {
     // (makes it look like the byte written was received from the second PS/2 port)
     send_cmd(0xD3, data);
   }

   void write_byte_to_second_ps2_port_input_buffer(uint8_t data) {
     // (sends next byte to the second PS/2 port)
     send_cmd(0xD4, data);
   }

   void pulse_reset_line_low(bool pulse) {
     send_cmd(pulse?0xF0:0xF1);
   }

   // Command Byte  Meaning   Response Byte
   // 0xC1  Copy bits 0 to 3 of input port to status bits 4 to 7  None
   // 0xC2  Copy bits 4 to 7 of input port to status bits 4 to 7  None
   void flush_output_buffer() {
     while(read_status_byte().output_buffer_full)
       read_response();
   }
 }
}

void init_kb() {
	// 1. disable translation to scancode set 1 (XT), and enable scancode set 2 (AT)
	// TODO:

	//iodev::ps2::disable_first_port();
	//iodev::ps2::disable_second_port();

	// iodev::ps2::flush_output_buffer(); // reading the status byte while disabled causes fault somehow...
	// so just read data
	//for(uint8_t i = 0; i < 16; i++) // 16 cuz output buffer is 16
	//	iodev::ps2::read_response();

	//iodev::ps2::flush_output_buffer();
	//auto ccb = iodev::ps2::read_controller_configuration_byte();
	//assert(ccb.second_ps2_port_clock_enabled);
	//ccb.first_ps2_port_interrupt_enabled = 0;
	//ccb.second_ps2_port_interrupt_enabled = 0;
	//ccb.first_ps2_port_translation_enabled = 0;
	//iodev::ps2::write_controller_configuration_byte(ccb);


	// if(iodev::ps2::test_controller())
	//  printk(LOG "PS/2 self-test OK\n");
	// else
	//  printk(LOG "PS/2 self-test FAILED\n");

	// auto t = iodev::ps2::test_first_port();
	// if( t == iodev::ps2::port_test_t::test_passed )
	//  printk(LOG "PS/2 first port-test OK\n");
	// else
	//  printk(LOG "PS/2 first port-test FAILED\n");

	// t = iodev::ps2::test_second_port();
	// if( t == iodev::ps2::port_test_t::test_passed )
	//  printk(LOG "PS/2 second port-test OK\n");
	// else
	//  printk(LOG "PS/2 second port-test FAILED\n");


	//iodev::ps2::enable_second_port();
	//iodev::ps2::enable_first_port();
	irq_install_custom_handler(1,keyboard_handler);
}

int keyboard_handler(struct interrupt_frame *) {
	//printf("*");
	if(!(inportb(0x64) & 0x1)) // has data ?
	{
		//printf("!");
		return 0;
	}

	do
	{
		auto scancode = inportb(0x60);
		//if(scancode & 0x80) printf("^%c", currKB[ 0x7f & scancode ]);
		//else printf("v%c", currKB [ scancode ]);

		if (scancode & 0x80) // released
		{
			scancode &= 0x7F; // remove the release flag
			if(VK_LSHIFT == currKB[scancode] || VK_RSHIFT == currKB[scancode])
			{
				shift = 0;
				currKB = KeyboardUs;
			}
		}
		else
		{
			if(VK_LSHIFT == currKB[scancode] || VK_RSHIFT == currKB[scancode])
			{
				shift = 1;
				currKB = shiftKeyboardUs;
			}
			else
				readline_handler(currKB[scancode]);
		}
	} while(inportb(0x64) & 0x1);  // flush the buffer (and process it) (TODO: check codeset...)

	return 0;
}

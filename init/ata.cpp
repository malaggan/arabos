#include <asm.h>
#include <types.h>
#include <file_t.h>
#include <port.h>
#include <ata.h>

struct IDENTIFY_DEVICE_DATA {
	struct {
		uint16_t Reserved1 :1;
		uint16_t Retired3 :1;
		uint16_t ResponseIncomplete :1;			// Indicates that the response was incomplete.
		uint16_t Retired2 :3;
		uint16_t FixedDevice :1;
		uint16_t RemovableMedia :1;			// Indicates when set to 1 that the media is removable.
		uint16_t Retired1 :7;
		uint16_t DeviceType :1;				// Indicates when set to 1 that the device is an ATA device.
	} GeneralConfiguration;
	uint16_t NumCylinders;				// Indicates the number of cylinders on the device.
	uint16_t ReservedWord2;
	uint16_t NumHeads;					// Number of logical heads on the device.
	uint16_t Retired1[2];
	uint16_t NumSectorsPerTrack;			// Indicates the number of sectors per track.
	uint16_t VendorUnique1[3];				// Contains the first ID of the device's vendor.
	uint8_t SerialNumber[20];				// Contains the serial number of the device.
	uint16_t Retired2[2];
	uint16_t Obsolete1;
	uint8_t FirmwareRevision[8];			// Contains the revision number of the device's firmware.
	uint8_t ModelNumber[40];				// Contains the device's model number.
	uint8_t MaximumBlockTransfer;			// Contains the maximum number of blocks allowed in a single transfer.
	uint8_t VendorUnique2;				// Contains the second ID of the device's vendor.
	uint16_t ReservedWord48;
	struct {
		uint8_t ReservedByte49;
		uint8_t DmaSupported :1;			// Indicates that the device supports DMA operations.
		uint8_t LbaSupported :1;			// Indicates that the device supports logical block addressing.
		uint8_t IordyDisable :1;			// Indicates when set to 1 that I/O channel ready is disabled for the device.
		uint8_t IordySupported :1;			// Indicates when set to 1 that I/O channel ready is supported by the device.
		uint8_t Reserved1 :1;
		uint8_t StandybyTimerSupport :1;		// Indicates when set to 1 that the device supports standby timers.
		uint8_t Reserved2 :2;
		uint16_t ReservedWord50;
	} Capabilities;
	uint16_t ObsoleteWords51[2];
	uint16_t TranslationFieldsValid :3;			// Contains a bitfield whose bits indicate which of the bytes in the identify data package contain valid address translation information. For more information about how this bitfield is defined, see the ATA/ATAPI specification.
	uint16_t Reserved3 :13;
	uint16_t NumberOfCurrentCylinders;			// Indicates the number of cylinders on the device.
	uint16_t NumberOfCurrentHeads;			// Indicates the number of heads on the device.
	uint16_t CurrentSectorsPerTrack;			// Indicates the number of sectors per track.
	uint32_t CurrentSectorCapacity;			// Indicates the number of sectors on the device.
	uint8_t CurrentMultiSectorSetting;			// Indicates the multisector setting.
	uint8_t MultiSectorSettingValid :1;			// Indicates when TRUE that the multisector setting is valid.
	uint8_t ReservedByte59 :7;
	uint32_t UserAddressableSectors;			// Indicates the total number of user-addressable sectors.
	uint16_t ObsoleteWord62;
	uint16_t MultiWordDMASupport :8;			// Indicates which DMA modes the device supports.
	uint16_t MultiWordDMAActive :8;			// Indicates which DMA modes are currently selected.
	uint16_t AdvancedPIOModes :8;
	uint16_t ReservedByte64 :8;
	uint16_t MinimumMWXferCycleTime;			// Indicates the minimum multiword DMA transfer cycle time per word.
	uint16_t RecommendedMWXferCycleTime;		// Indicates the recommended multiword DMA transfer cycle time per word.
	uint16_t MinimumPIOCycleTime;			// Indicates the minimum PIO transfer cycle time without flow control.
	uint16_t MinimumPIOCycleTimeIORDY;			// Indicates the minimum PIO transfer cycle time with IORDY flow control.
	uint16_t ReservedWords69[6];
	uint16_t QueueDepth :5;				// Indicates the maximum queue depth.
	uint16_t ReservedWord75 :11;
	uint16_t ReservedWords76[4];
	uint16_t MajorRevision;				// Indicates the device's major revision number.
	uint16_t MinorRevision;				// Indicates the device's minor revision number.
	struct {
		uint16_t SmartCommands :1;
		uint16_t SecurityMode :1;
		uint16_t RemovableMediaFeature :1;
		uint16_t PowerManagement :1;
		uint16_t Reserved1 :1;
		uint16_t WriteCache :1;
		uint16_t LookAhead :1;
		uint16_t ReleaseInterrupt :1;
		uint16_t ServiceInterrupt :1;
		uint16_t DeviceReset :1;
		uint16_t HostProtectedArea :1;
		uint16_t Obsolete1 :1;
		uint16_t WriteBuffer :1;
		uint16_t ReadBuffer :1;
		uint16_t Nop :1;
		uint16_t Obsolete2 :1;
		uint16_t DownloadMicrocode :1;
		uint16_t DmaQueued :1;
		uint16_t Cfa :1;
		uint16_t AdvancedPm :1;
		uint16_t Msn :1;
		uint16_t PowerUpInStandby :1;
		uint16_t ManualPowerUp :1;
		uint16_t Reserved2 :1;
		uint16_t SetMax :1;
		uint16_t Acoustics :1;
		uint16_t BigLba :1;
		uint16_t DeviceConfigOverlay :1;
		uint16_t FlushCache :1;
		uint16_t FlushCacheExt :1;
		uint16_t Resrved3 :2;
		uint16_t SmartErrorLog :1;
		uint16_t SmartSelfTest :1;
		uint16_t MediaSerialNumber :1;
		uint16_t MediaCardPassThrough :1;
		uint16_t StreamingFeature :1;
		uint16_t GpLogging :1;
		uint16_t WriteFua :1;
		uint16_t WriteQueuedFua :1;
		uint16_t WWN64Bit :1;
		uint16_t URGReadStream :1;
		uint16_t URGWriteStream :1;
		uint16_t ReservedForTechReport :2;
		uint16_t IdleWithUnloadFeature :1;
		uint16_t Reserved4 :2;
	} CommandSetSupport;
	struct {
		uint16_t SmartCommands :1;			// Indicates when TRUE that the device supports the SMART feature set.
		uint16_t SecurityMode :1;			// Indicates when TRUE that the device supports the security mode feature set.
		uint16_t RemovableMediaFeature :1;
		uint16_t PowerManagement :1;			// Indicates when TRUE that the device supports the mandatory power management feature set.
		uint16_t Reserved1 :1;
		uint16_t WriteCache :1;				// Indicates when TRUE that the device supports a write cache.
		uint16_t LookAhead :1;				// Indicates when TRUE that the device supports lookahead.
		uint16_t ReleaseInterrupt :1;			// Indicates when TRUE that the device supports release interrupt.
		uint16_t ServiceInterrupt :1;			// Indicates when TRUE that the device supports service interrupt.
		uint16_t DeviceReset :1;			// Indicates when TRUE that the device supports the device reset command.
		uint16_t HostProtectedArea :1;			// Indicates when TRUE that the device supports the host protected area feature set.
		uint16_t Obsolete1 :1;
		uint16_t WriteBuffer :1;			// Indicates when TRUE that the device supports the write buffer command.
		uint16_t ReadBuffer :1;				// Indicates when TRUE that the device supports the read buffer command.
		uint16_t Nop :1;				// Indicates when TRUE that the device supports the NOP command.
		uint16_t Obsolete2 :1;
		uint16_t DownloadMicrocode :1;			// Indicates when TRUE that the device supports the DOWNLOAD MICROCODE command.
		uint16_t DmaQueued :1;				// Indicates when TRUE that the device supports READ/WRITE DMA QUEUED command.
		uint16_t Cfa :1;				// Indicates when TRUE that the device supports the CFA feature set.
		uint16_t AdvancedPm :1;				// Indicates when TRUE that the device supports the advanced power management feature set.
		uint16_t Msn :1;				// Indicates when TRUE that the device supports the media status notification feature set.
		uint16_t PowerUpInStandby :1;			// Indicates when TRUE that the device supports power-up in standby feature set.
		uint16_t ManualPowerUp :1;			// Indicates when TRUE that the device supports the SET FEATURES subcommand required to spin up the device after power-up.
		uint16_t Reserved2 :1;
		uint16_t SetMax :1;				// Indicates when TRUE that the device supports the SET MAX security extension command.
		uint16_t Acoustics :1;				// Indicates when TRUE that the device supports the automatic acoustic management feature set.
		uint16_t BigLba :1;				// Indicates when TRUE that the device supports the 48-bit address feature set.
		uint16_t DeviceConfigOverlay :1;
		uint16_t FlushCache :1;
		uint16_t FlushCacheExt :1;
		uint16_t Resrved3 :2;
		uint16_t SmartErrorLog :1;
		uint16_t SmartSelfTest :1;
		uint16_t MediaSerialNumber :1;
		uint16_t MediaCardPassThrough :1;
		uint16_t StreamingFeature :1;
		uint16_t GpLogging :1;
		uint16_t WriteFua :1;
		uint16_t WriteQueuedFua :1;
		uint16_t WWN64Bit :1;
		uint16_t URGReadStream :1;
		uint16_t URGWriteStream :1;
		uint16_t ReservedForTechReport :2;
		uint16_t IdleWithUnloadFeature :1;
		uint16_t Reserved4 :2;
	} CommandSetActive;
	uint16_t UltraDMASupport :8;
	uint16_t UltraDMAActive :8;				// Contains a bitmap indicating which ultraDMA modes are selected.
	uint16_t ReservedWord89[4];
	uint16_t HardwareResetResult;			// Indicates the result of a hardware reset. For more information about the values assigned to this member, see the ATA/ATAP specification.
	uint16_t CurrentAcousticValue :8;			// Indicates the current acoustic management value.
	uint16_t RecommendedAcousticValue :8;		// Contain the device vendor's recommended acoustic management value.
	uint16_t ReservedWord95[5];
	uint32_t Max48BitLBA[2];				// Contains the maximum user LBA for the 48-bit address feature set.
	uint16_t StreamingTransferTime;
	uint16_t ReservedWord105;
	struct {
		uint16_t LogicalSectorsPerPhysicalSector :4;
		uint16_t Reserved0 :8;
		uint16_t LogicalSectorLongerThan256Words :1;
		uint16_t MultipleLogicalSectorsPerPhysicalSector :1;
		uint16_t Reserved1 :2;
	} PhysicalLogicalSectorSize;
	uint16_t InterSeekDelay;
	uint16_t WorldWideName[4];
	uint16_t ReservedForWorldWideName128[4];
	uint16_t ReservedForTlcTechnicalReport;
	uint16_t WordsPerLogicalSector[2];
	struct {
		uint16_t ReservedForDrqTechnicalReport :1;
		uint16_t WriteReadVerifySupported :1;
		uint16_t Reserved01 :11;
		uint16_t Reserved1 :2;
	} CommandSetSupportExt;
	struct {
		uint16_t ReservedForDrqTechnicalReport :1;
		uint16_t WriteReadVerifyEnabled :1;
		uint16_t Reserved01 :11;
		uint16_t Reserved1 :2;
	} CommandSetActiveExt;
	uint16_t ReservedForExpandedSupportandActive[6];
	uint16_t MsnSupport :2;				// Indicates when TRUE that the device supports media status notification.
	uint16_t ReservedWord1274 :14;
	struct {
		uint16_t SecuritySupported :1;
		uint16_t SecurityEnabled :1;
		uint16_t SecurityLocked :1;
		uint16_t SecurityFrozen :1;
		uint16_t SecurityCountExpired :1;
		uint16_t EnhancedSecurityEraseSupported :1;
		uint16_t Reserved0 :2;
		uint16_t SecurityLevel :1;
		uint16_t Reserved1 :7;
	} SecurityStatus;					// // Contains a bitmap that indicates the security status. For more information about the meaning of each individual bit, see the ATA/ATAPI specification.
	uint16_t ReservedWord129[31];
	struct {
		uint16_t MaximumCurrentInMA2 :12;
		uint16_t CfaPowerMode1Disabled :1;
		uint16_t CfaPowerMode1Required :1;
		uint16_t Reserved0 :1;
		uint16_t Word160Supported :1;
	} CfaPowerModel;
	uint16_t ReservedForCfaWord161[8];
	struct {
		uint16_t SupportsTrim :1;
		uint16_t Reserved0 :15;
	} DataSetManagementFeature;
	uint16_t ReservedForCfaWord170[6];
	uint16_t CurrentMediaSerialNumber[30];
	uint16_t ReservedWord206;
	uint16_t ReservedWord207[2];
	struct {
		uint16_t AlignmentOfLogicalWithinPhysical :14;
		uint16_t Word209Supported :1;
		uint16_t Reserved0 :1;
	} BlockAlignment;
	uint16_t WriteReadVerifySectorCountMode3Only[2];
	uint16_t WriteReadVerifySectorCountMode2Only[2];
	struct {
		uint16_t NVCachePowerModeEnabled :1;
		uint16_t Reserved0 :3;
		uint16_t NVCacheFeatureSetEnabled :1;
		uint16_t Reserved1 :3;
		uint16_t NVCachePowerModeVersion :4;
		uint16_t NVCacheFeatureSetVersion :4;
	} NVCacheCapabilities;
	uint16_t NVCacheSizeLSW;
	uint16_t NVCacheSizeMSW;
	uint16_t NominalMediaRotationRate;
	uint16_t ReservedWord218;
	struct {
		uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
		uint8_t Reserved;
	} NVCacheOptions;
	uint16_t ReservedWord220[35];
	uint16_t Signature :8;				// Indicates the disk signature.
	uint16_t CheckSum :8;				// Indicates the checksum.
} __attribute__((packed));

constexpr uint16_t REG_DATA         = 0X1F0;
constexpr uint16_t REG_FEATURES     = 0X1F1;
constexpr uint16_t REG_ERROR        = 0X1F1;
constexpr uint16_t REG_SECTOR_COUNT	= 0X1F2;
constexpr uint16_t REG_LBA_LO       = 0X1F3;
constexpr uint16_t REG_LBA_MID      = 0X1F4;
constexpr uint16_t REG_LBA_HI       = 0X1F5;
constexpr uint16_t REG_DEVICE       = 0X1F6;
constexpr uint16_t REG_HEAD         = 0X1F6;
constexpr uint16_t REG_STATUS       = 0X1F7;
constexpr uint16_t REG_COMMAND      = 0X1F7;

// status
constexpr uint8_t FLAG_BUSY          = 0b10000000;
constexpr uint8_t FLAG_DEVICE_READY  = 0b01000000;	// RDRY
constexpr uint8_t FLAG_DRQ           = 0b00001000;	// if DRQ is 0, the device doesn't have data for us

constexpr uint8_t CMD_IDENTIFY_DRIVE = 0xEC;
constexpr uint8_t CMD_CACHE_FLUSH    = 0xE7;
constexpr uint8_t CMD_READ_SECTORS   = 0x20;
constexpr uint8_t CMD_WRITE_SECTORS  = 0x30;
constexpr uint8_t MASTER             = 0x0;
constexpr uint8_t SLAVE              = 0x10;

void wait_not_busy()        {while(  inportb(REG_STATUS) & FLAG_BUSY        ) ;}
void wait_ready()           {while(!(inportb(REG_STATUS) & FLAG_DEVICE_READY));}
void wait_for_data()        {while(!(inportb(REG_STATUS) & FLAG_DRQ         ));}
void select_master_device()	{outportb(REG_DEVICE, MASTER);}
void identify_drive()
{
	wait_not_busy();
	cli();
	wait_ready();
	select_master_device();
	outportb(REG_COMMAND,CMD_IDENTIFY_DRIVE);
	wait_for_data();

	IDENTIFY_DEVICE_DATA data;
	__asm__ __volatile__ (
		"cld \n"
		"rep insw \n" :: "d"(REG_DATA), "D"(&data), "c"(256): // size: 512: 256*2 (cuz "word")
		"cc","memory");

	sti();
	printf("%s\n",data.GeneralConfiguration.ResponseIncomplete?"Response Incomplete":"Response Complete");
	printf("%s\n",data.GeneralConfiguration.RemovableMedia?"Removable Media":"Persistent Media");
	printf("%s\n",data.GeneralConfiguration.DeviceType?"ATA Device":"Non-ATA device");
	printf("Cylinders: %d, heads: %d, sectors per track: %d\n", data.NumCylinders, data.NumHeads, data.NumSectorsPerTrack);
	printf("vendor %x:%x:%x %x\n",data.VendorUnique1[0],data.VendorUnique1[1],data.VendorUnique1[2],data.VendorUnique2);
	printf("%s\n",data.Capabilities.DmaSupported?"DMA supported":"DMA not supported");
	printf("%s\n",data.Capabilities.LbaSupported?"LBA supported":"LBA not supported");
	printf("Cylinders: %d, heads: %d, sectors per track: %d, sector capacity: %d\n", data.NumberOfCurrentCylinders, data.NumberOfCurrentHeads, data.CurrentSectorsPerTrack, data.CurrentSectorCapacity);
	printf("Device revision number %d:%d\n", data.MajorRevision, data.MinorRevision);
	printf("Model number: \""); // endianess issues
	for(int i = 0; i < 40; i+=2)
		printf("%c%c", data.ModelNumber[i+1], data.ModelNumber[i]);
	printf("\n");

	printf("Serial number: "); // endianess issues
	for(int i = 0; i < 20; i+=2)
		printf("%c%c", data.SerialNumber[i+1], data.SerialNumber[i]);
	printf("\n");

	printf("Firmware Revision: "); // endianess issues
	for(int i = 0; i < 8; i+=2)
		printf("%c%c", data.FirmwareRevision[i+1], data.FirmwareRevision[i]);
	printf("\n");


	// uint16_t d[256];
	//uint16_t * a;
	// file_t ROOT =hd.blocks[0].change_type<file_t>("/",0777,file_type::D,3,false);
	// uint16_t* b=reinterpret_cast<uint16_t*>(&ROOT);
	// file_t* p1 = reinterpret_cast<file_t*>(b);

	// write(b,0);
	// read(d,0);
	// printf("name :%d\n",( reinterpret_cast<file_t*>(b))->inode);
	// printf("name :%s\n",( reinterpret_cast<file_t*>(b))->name.c_str());
	// file_t* p1 = reinterpret_cast<file_t*>(d);
	//  for(int i = 0; i < 10; i++)
	// printf("read: %s\n",p1->name.c_str());
}

void read_sector(uint16_t data[256], sector_t sect_num)
{
	wait_not_busy();
	cli();
	wait_ready();

	uint8_t slavebit = 0;
	uint32_t LBA = sect_num;  // 28 bits
	uint8_t count = 1; // read one sector
	// Send 0xE0 for the "master" or 0xF0 for the "slave", ORed with the highest 4 bits of the LBA to port 0x1F6:
	// Note on the "magic bits" sent to port 0x1f6: Bit 6 (value = 0x40) is the LBA bit. This must be set for either LBA28 or LBA48 transfers. It must be clear for CHS transfers. Bits 7 and 5 are obsolete for current ATA drives, but must be set for backwards compatibility with very old (ATA1) drives.
	constexpr uint8_t master = 0xE0;
	constexpr uint8_t slave = 0xF0;

	outportb(REG_DEVICE, master | ((LBA >> 24) & 0x0F));
	// Send a NULL byte to port 0x1F1, if you like (it is ignored and wastes lots of CPU time):
	outportb(REG_ERROR, 0x00);
	outportb(REG_SECTOR_COUNT, count);
	outportb(REG_LBA_LO , static_cast<uint8_t>(LBA));
	outportb(REG_LBA_MID, static_cast<uint8_t>(LBA >> 8));
	outportb(REG_LBA_HI , static_cast<uint8_t>(LBA >> 16));
	outportb(REG_COMMAND, CMD_READ_SECTORS);
	wait_for_data();
	// Wait for an IRQ or poll.
	// Transfer 256 16-bit values, a uint16_t at a time, into your buffer from I/O port 0x1F0. (In assembler, REP INSW works well for this.)
	__asm__ __volatile__ (
		"cld \n"
		"rep insw \n" :: "d"(REG_DATA), "D"(data), "c"(256): // read 256 "words"
		"cc","memory");
	// Then loop back to waiting for the next IRQ (or poll again -- see next note) for each successive sector.

	// Note for polling PIO drivers: After transferring the last uint16_t of a PIO data block to the data IO port, give the drive a 400ns delay to reset its DRQ bit (and possibly set BSY again, while emptying/filling its buffer to/from the drive).

	sti();
}

void write_sector(uint16_t const data[256], sector_t sect_num)
{
	wait_not_busy();
	cli();
	wait_ready();

	uint8_t slavebit = 0;
	uint32_t LBA = sect_num;  // 28 bits
	uint8_t count = 1; // read one sector
	// Send 0xE0 for the "master" or 0xF0 for the "slave", ORed with the highest 4 bits of the LBA to port 0x1F6:
	// Note on the "magic bits" sent to port 0x1f6: Bit 6 (value = 0x40) is the LBA bit. This must be set for either LBA28 or LBA48 transfers. It must be clear for CHS transfers. Bits 7 and 5 are obsolete for current ATA drives, but must be set for backwards compatibility with very old (ATA1) drives.
	constexpr uint8_t master = 0xE0;
	constexpr uint8_t slave = 0xF0;

	outportb(REG_DEVICE, master | ((LBA >> 24) & 0x0F));
	// Send a NULL byte to port 0x1F1, if you like (it is ignored and wastes lots of CPU time):
	outportb(REG_ERROR, 0x00);
	outportb(REG_SECTOR_COUNT, count);
	outportb(REG_LBA_LO , static_cast<uint8_t>(LBA));
	outportb(REG_LBA_MID, static_cast<uint8_t>(LBA >> 8));
	outportb(REG_LBA_HI , static_cast<uint8_t>(LBA >> 16));
	outportb(REG_COMMAND, CMD_WRITE_SECTORS);
	// uint16_t data[256] = {0xdead,0xbeef,0xdead,0xbeef,0xdead,0xbeef,0xdead,0xbeef,0xdead,0xbeef,0xdead,0xbeef};
	for(int i=0;i<256;i++)
		outportw(REG_DATA, data[i]);
	outportb(REG_COMMAND, CMD_CACHE_FLUSH);
	sti();
}

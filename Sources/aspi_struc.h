#if !defined(ASPI_STRUC_H_INCLUDED_)
#define ASPI_STRUC_H_INCLUDED_

//***********************************************************************//
// SPTI
//***********************************************************************//

//-------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------

// method codes
#define  METHOD_BUFFERED     0
#define  METHOD_IN_DIRECT    1
#define  METHOD_OUT_DIRECT   2
#define  METHOD_NEITHER      3

// file access values
#define  FILE_ANY_ACCESS      0
//#define  FILE_READ_ACCESS     (0x0001)
//#define  FILE_WRITE_ACCESS    (0x0002)


#define IOCTL_CDROM_BASE   0x00000002
#define IOCTL_SCSI_BASE    0x00000004

// constants for DataIn member of SCSI_PASS_THROUGH* structures
#define  SCSI_IOCTL_DATA_OUT          0
#define  SCSI_IOCTL_DATA_IN           1
#define  SCSI_IOCTL_DATA_UNSPECIFIED  2

// Standard IOCTL define
#define CTL_CODE( DevType, Function, Method, Access ) (                 \
    ((DevType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_CDROM_READ_TOC          CTL_CODE(IOCTL_CDROM_BASE, 0x0000, METHOD_BUFFERED, FILE_READ_ACCESS )
#define IOCTL_CDROM_GET_LAST_SESSION  CTL_CODE(IOCTL_CDROM_BASE, 0x000E, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_SCSI_PASS_THROUGH         CTL_CODE( IOCTL_SCSI_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define IOCTL_SCSI_MINIPORT             CTL_CODE( IOCTL_SCSI_BASE, 0x0402, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define IOCTL_SCSI_GET_INQUIRY_DATA     CTL_CODE( IOCTL_SCSI_BASE, 0x0403, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SCSI_GET_CAPABILITIES     CTL_CODE( IOCTL_SCSI_BASE, 0x0404, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SCSI_PASS_THROUGH_DIRECT  CTL_CODE( IOCTL_SCSI_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define IOCTL_SCSI_GET_ADDRESS          CTL_CODE( IOCTL_SCSI_BASE, 0x0406, METHOD_BUFFERED, FILE_ANY_ACCESS )


//-------------------------------------------------------------------------
// Typedefs
//-------------------------------------------------------------------------

typedef struct {
  USHORT Length;
  UCHAR  ScsiStatus;
  UCHAR  PathId;
  UCHAR  TargetId;
  UCHAR  Lun;
  UCHAR  CdbLength;
  UCHAR  SenseInfoLength;
  UCHAR  DataIn;
  ULONG  DataTransferLength;
  ULONG  TimeOutValue;
  ULONG  DataBufferOffset;
  ULONG  SenseInfoOffset;
  UCHAR  Cdb[16];
} SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;

typedef struct
{
	USHORT Length;
	UCHAR  ScsiStatus;
	UCHAR  PathId;
	UCHAR  TargetId;
	UCHAR  Lun;
	UCHAR  CdbLength;
	UCHAR  SenseInfoLength;
	UCHAR  DataIn;
	ULONG  DataTransferLength;
	ULONG  TimeOutValue;
	PVOID  DataBuffer;
	ULONG  SenseInfoOffset;
	UCHAR  Cdb[16];
}	SCSI_PASS_THROUGH_DIRECT,
	*PSCSI_PASS_THROUGH_DIRECT;

typedef struct 
{
	SCSI_PASS_THROUGH_DIRECT spt;
	ULONG Filler;
	UCHAR ucSenseBuf[32];
}	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,
	*PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;


//***********************************************************************//
// ASPI
//***********************************************************************//

//-------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------

// SCSI MISCELLANEOUS EQUATES
#define SENSE_LEN                 14     // Default sense buffer length
#define SRB_DIR_SCSI              0x00   // Direction determined by SCSI
#define SRB_POSTING               0x01   // Enable ASPI posting
#define SRB_ENABLE_RESIDUAL_COUNT 0x04   // Enable residual byte count
                                         // reporting
#define SRB_DIR_IN                0x08   // Transfer from SCSI target to
                                         // host
#define SRB_DIR_OUT               0x10   // Transfer from host to SCSI
                                         // target
#define SRB_EVENT_NOTIFY          0x40   // Enable ASPI event notification
#define RESIDUAL_COUNT_SUPPORTED  0x02   // Extended buffer flag
#define MAX_SRB_TIMEOUT       1080001u   // 30 hour maximum timeout in sec
#define DEFAULT_SRB_TIMEOUT   1080001u   // use max.timeout by default

// ASPI command definitions
#define SC_HA_INQUIRY             0x00   // Host adapter inquiry
#define SC_GET_DEV_TYPE           0x01   // Get device type
#define SC_EXEC_SCSI_CMD          0x02   // Execute SCSI command
#define SC_ABORT_SRB              0x03   // Abort an SRB
#define SC_RESET_DEV              0x04   // SCSI bus device reset
#define SC_SET_HA_PARMS           0x05   // Set HA parameters
#define SC_GET_DISK_INFO          0x06   // Get Disk
#define SC_RESCAN_SCSI_BUS        0x07   // Rebuild SCSI device map
#define SC_GETSET_TIMEOUTS        0x08   // Get/Set target timeouts


// SRB Status
#define SS_PENDING                0x00   // SRB being processed
#define SS_COMP                   0x01   // SRB completed without error
#define SS_ABORTED                0x02   // SRB aborted
#define SS_ABORT_FAIL             0x03   // Unable to abort SRB
#define SS_ERR                    0x04   // SRB completed with error
#define SS_INVALID_CMD            0x80   // Invalid ASPI command
#define SS_INVALID_HA             0x81   // Invalid host adapter number
#define SS_NO_DEVICE              0x82   // SCSI device not installed
#define SS_INVALID_SRB            0xE0   // Invalid parameter set in SRB
#define SS_OLD_MANAGER            0xE1   // ASPI manager doesn't support
                                         // windows
#define SS_BUFFER_ALIGN           0xE1   // Buffer not aligned (replaces
                                         // SS_OLD_MANAGER in Win32)
#define SS_ILLEGAL_MODE           0xE2   // Unsupported Windows mode
#define SS_NO_ASPI                0xE3   // No ASPI managers
#define SS_FAILED_INIT            0xE4   // ASPI for windows failed init
#define SS_ASPI_IS_BUSY           0xE5   // No resources available to
                                         // execute command
#define SS_BUFFER_TO_BIG          0xE6   // Buffer size too big to handle
#define SS_BUFFER_TOO_BIG         0xE6   // Correct spelling of 'too'
#define SS_MISMATCHED_COMPONENTS  0xE7   // The DLLs/EXEs of ASPI don't
                                         // version check
#define SS_NO_ADAPTERS            0xE8   // No host adapters to manager
#define SS_INSUFFICIENT_RESOURCES 0xE9   // Couldn't allocate resources
                                         // needed to init
#define SS_ASPI_IS_SHUTDOWN       0xEA   // Call came to ASPI after
                                         // PROCESS_DETACH
#define SS_BAD_INSTALL            0xEB   // The DLL or other components
                                         // are installed wrong

// Host Adapter Status
#define HASTAT_OK                 0x00   // No error detected by HA
#define HASTAT_SEL_TO             0x11   // Selection Timeout
#define HASTAT_DO_DU              0x12   // Data overrun/data underrun
#define HASTAT_BUS_FREE           0x13   // Unexpected bus free
#define HASTAT_PHASE_ERR          0x14   // Target bus phase sequence
#define HASTAT_TIMEOUT            0x09   // Timed out while SRB was
                                         // waiting to be processed
#define HASTAT_COMMAND_TIMEOUT    0x0B   // Adapter timed out while
                                         // processing SRB
#define HASTAT_MESSAGE_REJECT     0x0D   // While processing the SRB, the
                                         // adapter received a MESSAGE
#define HASTAT_BUS_RESET          0x0E   // A bus reset was detected
#define HASTAT_PARITY_ERROR       0x0F   // A parity error was detected
#define HASTAT_REQUEST_SENSE_FAILED 0x10 // The adapter failed in issuing



// SCSI
#define DTYPE_CDROM     0x05    // CD-ROM device




//-------------------------------------------------------------------------
// Typedefs
//-------------------------------------------------------------------------

// SRB - HOST ADAPTER INQUIRIY - SC_HA_INQUIRY (0)
typedef struct 
{
	BYTE     SRB_Cmd;           // 00/000 ASPI command code == SC_HA_INQUIRY 
	BYTE     SRB_Status;        // 01/001 ASPI command status byte           
	BYTE     SRB_HaID;          // 02/002 ASPI host adapter number           
	BYTE     SRB_Flags;         // 03/003 ASPI request flags                 
	DWORD    SRB_Hdr_Rsvd;      // 04/004 Reserved, must = 0                 
	BYTE     HA_Count;          // 08/008 Number of host adapters present    
	BYTE     HA_SCSI_ID;        // 09/009 SCSI ID of host adapter            
	BYTE     HA_ManagerId[16];  // 0a/010 String describing the manager      
	BYTE     HA_Identifier[16]; // 1a/026 String describing the host adapter 
	BYTE     HA_Unique[16];     // 2a/042 Host Adapter Unique parameters     
	WORD     HA_Rsvd1;          // 3a/058 Reserved, must = 0                 
	BYTE     pad[20];
}	SRB_HAInquiry,
	*PSRB_HAInquiry,
	FAR *LPSRB_HAInquiry;

// SRB - GET DEVICE TYPE - SC_GET_DEV_TYPE (1)
typedef struct
{
	BYTE     SRB_Cmd;           // 00/000 ASPI cmd code == SC_GET_DEV_TYPE   
	BYTE     SRB_Status;        // 01/001 ASPI command status byte           
	BYTE     SRB_HaID;          // 02/002 ASPI host adapter number           
	BYTE     SRB_Flags;         // 03/003 Reserved, must = 0                 
	DWORD    SRB_Hdr_Rsvd;      // 04/004 Reserved, must = 0                 
	BYTE     SRB_Target;        // 08/008 Target's SCSI ID                   
	BYTE     SRB_Lun;           // 09/009 Target's LUN number                
	BYTE     SRB_DeviceType;    // 0a/010 Target's peripheral device type    
	BYTE     SRB_Rsvd1;         // 0b/011 Reserved, must = 0                 
	BYTE     pad[68];
}	SRB_GDEVBlock,
	*PSRB_GDEVBlock,
	FAR *LPSRB_GDEVBlock;


// SRB - EXECUTE SCSI COMMAND - SC_EXEC_SCSI_CMD (2)
typedef struct
{
	BYTE     SRB_Cmd;           // 00/000 ASPI cmd code == SC_EXEC_SCSI_CMD  
	BYTE     SRB_Status;        // 01/001 ASPI command status byte           
	BYTE     SRB_HaID;          // 02/002 ASPI host adapter number           
	BYTE     SRB_Flags;         // 03/003 Reserved, must = 0                 
	DWORD    SRB_Hdr_Rsvd;      // 04/004 Reserved, must = 0                 
	BYTE     SRB_Target;        // 08/008 Target's SCSI ID                   
	BYTE     SRB_Lun;           // 09/009 Target's LUN                       
	WORD     SRB_Rsvd1;         // 0a/010 Reserved for alignment             
	DWORD    SRB_BufLen;        // 0c/012 Data Allocation Length             
	BYTE FAR *SRB_BufPointer;   // 10/016 Data Buffer Pointer                
	BYTE     SRB_SenseLen;      // 14/020 Sense Allocation Length            
	BYTE     SRB_CDBLen;        // 15/021 CDB Length                         
	BYTE     SRB_HaStat;        // 16/022 Host Adapter Status                
	BYTE     SRB_TargStat;      // 17/023 Target Status                      
	VOID FAR *SRB_PostProc;     // 18/024 Post routine                       
	BYTE     SRB_Rsvd2[20];     // 1c/028 Reserved, must = 0                 
	BYTE     CDBByte[16];       // 30/048 SCSI CDB                           
	BYTE SenseArea[SENSE_LEN+2]; // 40/064 Request Sense buffer              
}	SRB_ExecSCSICmd, *PSRB_ExecSCSICmd, FAR *LPSRB_ExecSCSICmd;


// SRB - BUS DEVICE RESET - SC_RESET_DEV (4)
typedef struct
{
  BYTE     SRB_Cmd;           // 00/000 ASPI cmd code == SC_RESET_DEV      
  BYTE     SRB_Status;        // 01/001 ASPI command status byte           
  BYTE     SRB_HaId;          // 02/002 ASPI host adapter number           
  BYTE     SRB_Flags;         // 03/003 Reserved, must = 0                 
  DWORD    SRB_Hdr_Rsvd;      // 04/004 Reserved                           
  BYTE     SRB_Target;        // 08/008 Target's SCSI ID                   
  BYTE     SRB_Lun;           // 09/009 Target's LUN number                
  BYTE     SRB_Rsvd1[12];     // 0A/010 Reserved for alignment             
  BYTE     SRB_HaStat;        // 16/022 Host Adapter Status                
  BYTE     SRB_TargStat;      // 17/023 Target Status                      
  VOID FAR *SRB_PostProc;     // 18/024 Post routine                       
  BYTE     SRB_Rsvd2[36];     // 1C/028 Reserved, must = 0                 
  BYTE     pad[16];
}	SRB_BusDeviceReset,
	*PSRB_BusDeviceReset,
	FAR *LPSRB_BusDeviceReset;


//
typedef struct tag_ASPI32BUFF
{
  PBYTE     AB_BufPointer;
  DWORD     AB_BufLen;
  DWORD     AB_ZeroFill;
  DWORD     AB_Reserved;
}	ASPI32BUFF,
	*PASPI32BUFF,
	FAR *LPASPI32BUFF;

//
typedef struct 
{
	BYTE      SRB_Cmd;
	BYTE      SRB_Status;
	BYTE      SRB_HaId;
	BYTE      SRB_Flags;
	DWORD     SRB_Hdr_Rsvd;
}	SRB,
	*PSRB,
	FAR *LPSRB;

//
typedef struct
{
	BYTE ha;
	BYTE tgt;
	BYTE lun;
	char desc[32];
}	ASPIDRV;

typedef struct
{
	char driveLetter;
	char desc[32];
}	NTDRV;

typedef union
{
	ASPIDRV  a;
	NTDRV    n;
}	DRIVESTRUCT;

#define MAX_DRIVE_LIST  8
typedef struct
{
	DRIVESTRUCT drive[MAX_DRIVE_LIST];
	int num;
} DRIVELIST;


#endif

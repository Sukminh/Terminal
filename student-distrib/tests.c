#include "tests.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* divErr
 * Manually creating division by zero Exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int divErr(){
	TEST_HEADER;

	int i;
	int a = 1;
	int b = 0;
	i = a/b;
	return FAIL;
}

/* pageFaultTest
 * Manually creating pageFault Exception by dereferencing NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int pageFaultTest(){
	int *p = NULL;
	*p = 1;
	return FAIL;
}

/*
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: ??????
 * Files: ??????
 */
static inline int invalidOpcode(){
	asm volatile("movl %cr6, %eax");
	return FAIL;
}

/*
 * sysCall()
 *   DESCRIPTION: check system call
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int syscall(){
	asm volatile("int $128"); // 128 == 0x80, index for System Call
	return FAIL;
}

/*
 * DebugException()
 *   DESCRIPTION: check debug exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int DebugException(){
	asm volatile("int $1"); // 1 index for DebugException
	return FAIL;
}

/*
 * NMIInterruptException()
 *   DESCRIPTION: check non-maskable interrupt exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int NMIInterruptException(){
	asm volatile("int $2"); // 2 index for NMIInterruptException
	return FAIL;
}

/*
 * BreakPointException()
 *   DESCRIPTION: check brekapoint exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int BreakpointException(){
	asm volatile("int $3"); // 3 index for BreakpointException
	return FAIL;
}

/*
 * OverflowException()
 *   DESCRIPTION: check overflow exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int OverflowException(){
	asm volatile("int $4"); // 4 index for OverflowException
	return FAIL;
}

/*
 * BoundRangeExceedException()
 *   DESCRIPTION: check bound range exceeded exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int BoundRangeExceededException(){
	asm volatile("int $5"); // 5 index for BoundRangeExceededException
	return FAIL;
}

/*
 * DeviceBusyException()
 *   DESCRIPTION: check device not available exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int DeviceBusyException(){
	asm volatile("int $7"); // 7 index for DeviceBusyException
	return FAIL;
}

/*
 * CoprocessorOverrunException()
 *   DESCRIPTION: check coprocessor overrun exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int CoprocessorOverrunException(){
	asm volatile("int $9"); // 9 index for CoprocessorOverrunException
	return FAIL;
}

/*
 * InvalidTSSException()
 *   DESCRIPTION: check invalid TSS exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int InvalidTSSException(){
	asm volatile("int $10"); // 10 index for InvalidTSSException
	return FAIL;
}

/*
 * SegmentnotPresentException()
 *   DESCRIPTION: check segment not present exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int SegmentnotPresentException(){
	asm volatile("int $11"); // 11 index for SegmentnotPresentException
	return FAIL;
}

/*
 * StackSegmentFaultException()
 *   DESCRIPTION: check stack segment fault exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int StackSegmentFaultException(){
	asm volatile("int $12"); // 12 index for StackSegmentFaultException
	return FAIL;
}

/*
 * GeneralProtectionException()
 *   DESCRIPTION: check general protection fault exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int GeneralProtectionException(){
	asm volatile("int $13"); // 13 index for GeneralProtectionException
	return FAIL;
}

/*
 * x87FPUException()
 *   DESCRIPTION: check x87 floating point exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int x87FPUException(){
	asm volatile("int $16"); // 16 index for x87FPUException
	return FAIL;
}

/*
 * AlighmentCheckException()
 *   DESCRIPTION: check alighment check exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int AlignmentCheckException(){
	asm volatile("int $17"); // 17 index for AlignmentCheckException
	return FAIL;
}

/*
 * MachineCheckException()
 *   DESCRIPTION: check machine check exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int MachineCheckException(){
	asm volatile("int $18"); // 18 index for MachineCheckException
	return FAIL;
}

/*
 * SIMDFloatingPointException()
 *   DESCRIPTION: check SIMD floating-point exception
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: Pass or Fail
 *   SIDE EFFECTS: None
 */ 
static inline int SIMDFloatingPointException(){
	asm volatile("int $19"); // 19 index for SIMDFloatingPointException
	return FAIL;
}

/*
 * Testing if we can dereference valid pointer
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPaging1(){
	TEST_HEADER;
	int a = 391;					//valid pointer value
	int *b = &a;
	int result = PASS;
	if (*b!=a){
		assertion_failure();
		result = FAIL;
	}
	return result;
}

/*
 * Testing if we can access memory that is set to present
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPagingb8000(){
	TEST_HEADER;
	int* a = (int*)0xb8000;			//check if memory is out of range
	int b;
	b = *a;
	return PASS;
}

/*
 * Testing if we can access memory that is kernel page (4mb)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPaging400000(){
	TEST_HEADER;
	int* a = (int*)0x400000;		//check if memory is out of range
	int b;
	b = *a;
	return PASS;
}

/*
 * Testing if we can access memory that is not set to present (3mb)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPaging300000(){
	TEST_HEADER;
	int* a = (int*)0x300000;		//check if memory is out of range 
	int b;
	b = *a;
	return FAIL;
}

/*
 * Testing if we can access memory that is at 0x0
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPaging0(){
	TEST_HEADER;
	int* a = (int*)0x0;				//check if memory is out of range
	int b;
	b = *a;
	return FAIL;
}

/*
 * Testing if we can access memory that is at 0x900000
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int testPaging900000(){
	TEST_HEADER;
	int* a = (int*)0x900000;		//check if memory is out of range
	int b;
	b = *a;
	return FAIL;
}

/*
 * Testing if PDE value is correct
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int PDEtest1(){
	TEST_HEADER;
	int result = PASS;
	uint32_t a = directory[1].addr;
	if (a!=0x400){						//check if it is at max val
		assertion_failure();
		result = FAIL;
	}
	return result;
}

/*
 * Testing if PTE value is correct
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging initalization and enabling
 * Files: paging.h/S
 */
int PTEtest1(){
	TEST_HEADER;
	int result = PASS;
	uint32_t a = pageTable[0xb8].present;		//check if PTE val at 184 is correct
	if (a!=1){
		assertion_failure();
		result = FAIL;
	}
	return result;
}

// add more tests here

/* Checkpoint 2 tests */
/*
 * Testing if rtc is working correctly by changing its frequency
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc
 * Files: rtc.h/c
 */
int rtc_test(){
	TEST_HEADER;
    int i, j;
	int result = 0;
	result += rtc_open(NULL);
	clear();
	// loop for different frequencies
	for (i = 2; i <= 1024; i*=2){					//power of 2
		result += rtc_write(NULL, &i, 4);			//number of bytes of the freq value (should be 4 because it is int)
		printf("%d Hz: ", i);
		for (j = 0; j < i*2; j++) {					//power of 2
			result += rtc_read(NULL, NULL, NULL);
			printf("1");
		}
		printf("\n");
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}
}

/*
 * Testing if rtc is returning -1 if rtc gets a freq that is not power of 2.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc
 * Files: rtc.h/c
 */
int rtc_invalidfreq_test(){
	TEST_HEADER;
	int i = 3;									//pass in invalid frequency value
    int32_t result = rtc_write(NULL, &i, 4);	//number of bytes of the freq value (should be 4 because it is int)
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}
}

/*
 * Testing if terminal_read, and terminal_write working correctly 
 * when n_byte of terminal_read is smaller than that of terminal_write
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read and terminal_write
 * Files: terminal.h/c
 */
int terminal_full_test(){
	TEST_HEADER;
	int8_t* buf[128];						//buffer of size 128 for keyboard
	int result = 0;
	result += terminal_open(NULL);
    while(1) {
		terminal_read(NULL, buf, 128);		//write 128
		terminal_write(NULL, buf, 128);		//read 128
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}	
}


/*
 * Testing if terminal_read, and terminal_write working correctly 
 * when n_byte of terminal_read is smaller than that of terminal_write
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read and terminal_write
 * Files: terminal.h/c
 */
int terminal_small_read_test(){
	TEST_HEADER;
	int8_t* buf[128];						//buffer of size 128 for keyboard
	int result = 0;
	result += terminal_open(NULL);
    while(1) {
		terminal_read(NULL, buf, 20);		//read 20
		terminal_write(NULL, buf, 25);		//write 25
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}	
}

/*
 * Testing if terminal_read, and terminal_write working correctly 
 * when n_byte of terminal_read is larger than that of terminal_write
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read and terminal_write
 * Files: terminal.h/c
 */
int terminal_large_read_test(){
	TEST_HEADER;
	int8_t* buf[128];						//buffer of size 128 for keyboard
	int result = 0;
	result += terminal_open(NULL);
    while(1) {
		terminal_read(NULL, buf, 128);		//read 128
		terminal_write(NULL, buf, 25);		//write 25
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}	
}

/*
 * Testing if terminal_read, and terminal_write working correctly 
 * when n_byte of terminal_read is larger than buf size
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read and terminal_write
 * Files: terminal.h/c
 */
int terminal_read_buf_size_test(){
	TEST_HEADER;
	int8_t* buf[128];						//buffer of size 128 for keyboard 
	int result = 0;
	result += terminal_open(NULL);
    while(1) {
		terminal_read(NULL, buf, 200);		//read 200
		terminal_write(NULL, buf, 25);		//write 25
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}	
}

/*
 * Testing if terminal_read, and terminal_write working correctly 
 * when n_byte of terminal_write is larger than buf size
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read and terminal_write
 * Files: terminal.h/c
 */
int terminal_write_buf_size_test(){
	TEST_HEADER;
	int8_t* buf[128];						//buffer of size 128 for keyboard
	int result = 0;
	result += terminal_open(NULL);
    while(1) {
		terminal_read(NULL, buf, 5);		//read 5
		terminal_write(NULL, buf, 300);		//write 300
	}
	if (result == 0) {
		return PASS;
	}
	else {
		return FAIL;
	}	
}

/*
 * Testing files in directory 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: files in directory
 * Files: fsys.h/c
 */
int read_directory_test(){
	TEST_HEADER;
	
	// test buffer w/ random size, here chosen 100(doesn't matter)
	uint8_t test_buffer[100];

	// flags to check return value(which tells dir has opened correctly, dir write disabled)
	int32_t correct_open_check_flag = -1;
	int32_t dir_read_check_flag = -1;
	int32_t dir_write_check_flag = -2;

	// dir name
	uint8_t* test_dirname = (uint8_t*)".";

	// open directory
	correct_open_check_flag = dir_open(test_dirname);
	if (correct_open_check_flag == -1){
		// directory not opened correctly, it should've returend 0 on successful dir. open
		return FAIL;
	}

	// read directory
	dir_read_check_flag = dir_read();
	if (dir_read_check_flag == -1){
		// directory not read correctly, it should've returend 0 on successful dir. read
		return FAIL;
	}

	// write directory(unused read only filesystem), if dir write called(not supported cmd.)
	dir_write_check_flag = dir_write((int32_t)3, test_buffer, (int32_t)10);
	if (dir_write_check_flag != -1){
		// (unused read only filesystem), so flag should be -1
		return FAIL;
	}

	return PASS;
}

/*
 * Reading files in directory 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: read files in directory
 * Files: fsys.h/c
 */
int read_file_test(){
	TEST_HEADER;

	// index variable
	int j;
	// buffer that will be used as basket for file content characters
	// 40000 is used temporary for these test cases as max # of char don't exceed 40000
	uint8_t buf[40000];

	// flags to check return value(which tells file has opened correctly, file write disabled)
	int32_t correct_open_check_flag = -1;
	int32_t file_read_check_flag = -2;
	int32_t file_write_check_flag = -2;

	// test buffer w/ random size, here chosen 100(doesn't matter)
	uint8_t test_buffer[100];

	// correct and existing files
	// uint8_t* test_filename = (uint8_t*)"cat";
	// uint8_t* test_filename = (uint8_t*)"counter";
	// uint8_t* test_filename = (uint8_t*)"fish";
	// uint8_t* test_filename = (uint8_t*)"frame0.txt";
	// uint8_t* test_filename = (uint8_t*)"frame1.txt";
	uint8_t* test_filename = (uint8_t*)"grep";
	// uint8_t* test_filename = (uint8_t*)"hello";
	// uint8_t* test_filename = (uint8_t*)"ls";
	// uint8_t* test_filename = (uint8_t*)"pingpong";
	// uint8_t* test_filename = (uint8_t*)"shell";
	// uint8_t* test_filename = (uint8_t*)"sigtest";
	// uint8_t* test_filename = (uint8_t*)"syserr";
	// uint8_t* test_filename = (uint8_t*)"testprint";
	// uint8_t* test_filename = (uint8_t*)"verylargetextwithverylongname.txt";

	// incorrect file(name)
	// uint8_t* test_filename = (uint8_t*)"abcd";

	// open file
	correct_open_check_flag = fs_open(test_filename);
	if (correct_open_check_flag == -1){
		// file not opened correctly, it should've returend 0 on successful file open
		return FAIL;
	}

	// clear the buffer
	for (j=0; j<40000; j++){		//specify 40000 for # of bytes to read as max # of char don't exceed 40000
		// clear with 0x00, null
		buf[j] = 0x00;
	}

	// read the file, specify 40000 for # of bytes to read as max # of char don't exceed 40000
	// buf will be filled up after this function is finished
	file_read_check_flag = fs_read(0, buf, 40000); 
	if (file_read_check_flag == -1){
		// non-existent file
		printf("non-existent file: ");
		puts((int8_t*)test_filename);
		return FAIL;
	}

	// for each char in buffer, print out the char by char if it's not null
	for (j=0; j<40000; j++){		//specify 40000 for # of bytes to read as max # of char don't exceed 40000
		// if it's null, skip
		if (buf[j] == '\0'){
			continue;
		}
		// print out char
		putc(buf[j]);
	}

	printf("\n");
	printf("file_name: ");
	puts((int8_t*)test_filename);
	
	// write directory(unused read only filesystem), if dir write called(not supported cmd.)
	file_write_check_flag = fs_write((int32_t)3, test_buffer, (int32_t)10);
	if (file_write_check_flag != -1){
		// (unused read only filesystem), so flag should be -1
		return FAIL;
	}

	return PASS;

	// Print everything but the first three lines (hex 0x30 bytes) of file.
	// % xxd -s 0x30 file
	// Print 3 lines (hex 0x30 bytes) from the end of file.
	// % xxd -s -0x30 file
}



/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	 //clear();
	// CHECKPOINT 1

	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("Dereference Valid", testPaging1());
	// TEST_OUTPUT("testPagingb8000", testPagingb8000());
	// TEST_OUTPUT("testPaging400000", testPaging400000());
	// TEST_OUTPUT("PDEtest1", PDEtest1());
	// TEST_OUTPUT("PTEtest1", PTEtest1());

	/*Out of Bounds for Paging, should result in Page Fault*/
	
	// TEST_OUTPUT("testPaging300000", testPaging300000());
	// TEST_OUTPUT("testPaging0", testPaging0());
	// TEST_OUTPUT("testPaging900000", testPaging900000());

	/*Interrupt Testing, will blue screen*/

	// TEST_OUTPUT("divErr", divErr());
	// TEST_OUTPUT("pageFault", pageFaultTest());
	// TEST_OUTPUT("invalidOpcode", invalidOpcode());
	// TEST_OUTPUT("syscall", syscall());
	// TEST_OUTPUT("DebugException", DebugException());
	// TEST_OUTPUT("NMIInterruptException", NMIInterruptException());
	// TEST_OUTPUT("BreakpointException", BreakpointException());
	// TEST_OUTPUT("OverflowException", OverflowException());
	// TEST_OUTPUT("BoundRangeExceededException", BoundRangeExceededException());
	// TEST_OUTPUT("DeviceBusyException", DeviceBusyException());
	// TEST_OUTPUT("CoprocessorOverrunException", CoprocessorOverrunException());
	// TEST_OUTPUT("InvalidTSSException", InvalidTSSException());
	// TEST_OUTPUT("SegmentnotPresentException", SegmentnotPresentException());
	// TEST_OUTPUT("StackSegmentFaultException", StackSegmentFaultException());
	// TEST_OUTPUT("GeneralProtectionException", GeneralProtectionException());
	// TEST_OUTPUT("x87FPUException", x87FPUException());
	// TEST_OUTPUT("AlignmentCheckException", AlignmentCheckException());
	// TEST_OUTPUT("MachineCheckException", MachineCheckException());
	// TEST_OUTPUT("SIMDFloatingPointException", SIMDFloatingPointException());
	//test_exceptions1();
	// launch your tests here

	// CHECKPOINT 2

	// TEST_OUTPUT("RTC Test", rtc_test());
	//TEST_OUTPUT("RTC Invalid Frequency Test", rtc_invalidfreq_test());
	//TEST_OUTPUT("Terminal Full Test", terminal_full_test());
	//TEST_OUTPUT("Terminal Small Read Driver Test", terminal_small_read_test());
	//TEST_OUTPUT("Terminal Large Read Driver Test", terminal_large_read_test());
	//TEST_OUTPUT("Terminal Read Buf Size Driver Test", terminal_read_buf_size_test());
	//TEST_OUTPUT("Terminal Write Buf Size Driver Test", terminal_write_buf_size_test());
	//TEST_OUTPUT("Read Directory Test", read_directory_test());
	// TEST_OUTPUT("Read File Test", read_file_test());



}

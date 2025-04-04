#pragma once
#include <string>

const int KB = 1024;
const int LOG_MAX_SIZE = 10 * KB;
const int FUNC_MAX_LEN = 30;

// for logging
const std::string func_execute_write = "executeWrite()";
const std::string func_execute_read = "executeRead()";
const std::string func_execute_fullwrite = "executeFullWrite()";
const std::string func_execute_fullread = "executeFullRead()";
const std::string func_execute_erase = "executeErase()";
const std::string func_execute_erase_range = "executeEraseRange()";
const std::string func_execute_flush = "executeFlush()";
const std::string status_start = "started";
const std::string status_finish = "finished";

const std::string write_done = "[Write] Done\n";
const std::string read_done = "[Read] ";
const std::string fullwrite_done = "[FullWrite] Done\n";
const std::string flush_done = "[Flush] Done\n";
const std::string erase_done = "[Erase] ";

const std::string invalid_command = "INVALID COMMAND\n";
const std::string err_too_big_size = "[Invalid] Size is too big\n";
const std::string err_start_bigger_than_end =
    "[Invalid] Start LBA bigger than End LBA\n";

const int MAX_LBA = 100;
const int MAX_SIZE = 10;

const std::string output_file_name = "ssd_output.txt";

const std::string help_command =
    R"(Best Reviewers (A community of individuals who aspire to be the best reviewers) 
- byeongun.ko [Team Leader]
- yuz010.kim
- yunje.kim	
- hjy.park

[Commands]
- write LBA Value : Writes the specified Value to the given LBA.
- read LBA : Reads the Value from the given LBA.
- exit : Exits the Shell Test.
- fullwrite Value : Writes the specified Value to all LBAs.
- fullread : Reads values from all LBAs.
- erase LBA Size : Erase from LBA to Size.
- erase_range StartLBA EndLBA : Erase from between StartLBA and EndLBA.
- flush : Executes all commands in the buffer.
- shell_scripts.txt : Run all test.
- 1_FullWriteAndReadCompare : Run 1st test.
- 2_PartialLBAWrite : Run 2nd test.
- 3_WriteReadAging : Run 3rd test.
- 4_EraseAndWriteAging : Run 4th test.
)";

const std::string script_file_name = "shell_scripts.txt";
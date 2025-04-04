#pragma once
#include <string>

const int KB = 1024;
const int LOG_MAX_SIZE = 10 * KB;
const int FUNC_MAX_LEN = 30;
const std::string write_done = "[Write] Done\n";
const std::string read_done = "[Read] ";
const std::string fullwrite_done = "[FullWrite] Done\n";
const std::string flush_done = "[Flush] Done\n";
const std::string erase_done = "[Erase] ";

const std::string invalid_command = "INVALID COMMAND\n";
const std::string err_too_big_size = "[Error] Size is too big\n";
const std::string err_start_bigger_than_end =
    "[Error] Start LBA bigger than End LBA\n";

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
)";
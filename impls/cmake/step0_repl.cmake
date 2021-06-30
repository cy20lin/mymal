# https://stackoverflow.com/questions/64147396/how-to-read-from-standard-input-stdin-file-in-a-cmake-script
# https://unix.stackexchange.com/questions/36403/how-portable-are-dev-stdin-dev-stdout-and-dev-stderr


# function(read)
#     if(WIN32)
#         set(STDIN "CON")
#     else()
#         set(STDIN "/dev/fd/0")
#     endif()
# endfunction()
# file(READ "/dev/fd/0" x)
# execute_process(COMMAND "/bin/cat" TIMEOUT 2 OUTPUT_VARIABLE x)
set(cmd_readline "while read line; do echo \"\${line}\"; exit; done")
execute_process(COMMAND "/bin/sh" "-c" "${cmd_readline}" OUTPUT_VARIABLE x)

message(STATUS "${x}")
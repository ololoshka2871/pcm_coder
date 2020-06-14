
message(STATUS "Fixing SDL2 MSVS compability")

set(target_file ${__TARGET}/CmakeLists.txt)

file(READ ${target_file} content)
string(REPLACE 
	"list(APPEND EXTRA_LIBS user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32)"
	"list(APPEND EXTRA_LIBS vcruntime user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32)"
	content ${content})
file(WRITE  ${target_file} ${content})
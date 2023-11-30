
#include "test_arg.h"
#include "pico/platform.h"
#include "McuLog.h"

/* The variable below is not initialized during statup, and set by JRun using a test .JLinkScript,
 * writing the variable during HandleAfterFlashProg().
 */
static uint32_t __uninitialized_ram(unit_test_program_arg);

uint32_t TestArg_GetArgument(void) {
  McuLog_info("unit_test_program_arg: 0x%x @0x%x", unit_test_program_arg, &unit_test_program_arg);
  return unit_test_program_arg;
}
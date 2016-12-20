#ifndef _PCI_H_
#define _PCI_H_

/* Ref:  "The following code is made Public Domain out of Clicker's x86/cpu.h"
   From: http://wiki.osdev.org/CPUID
*/

/* macros */
/* constants */

/* structures */
/* variables */
/* functions */
unsigned int pci_config_read (unsigned short bus, unsigned short slot,
                              unsigned short func, unsigned short offset);
#endif

#ifndef _PCI_C_
#define _PCI_C_

#include "pci.h"
#include "io.h"

unsigned int pci_config_read (unsigned short bus, unsigned short slot,
                              unsigned short func, unsigned short offset)
{
    unsigned int address;
    unsigned int lbus = (unsigned int)bus;
    unsigned int lslot = (unsigned int)slot;
    unsigned int lfunc = (unsigned int)func;

    /* create configuration address as per Figure 1 */
    address = (unsigned int)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((unsigned int)0x80000000));

    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    //return ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    //return ((inl(0xCFC) >> ((offset & 2) * 8)));
    return ((inl(0xCFC) >> ((offset) * 8)));
}


#endif

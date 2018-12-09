# LC-SCSI
Low-cost SCSI emulator project based around the STM32.

### Status: Planning - check the wiki

## Aims: 
 * Emulate SCSI HDD Target, SCSI 1 
 * SD Card for storage
 * Low Cost, target BOM <$10USD
 * STM32/GD32 as the target controller
 * simple code with lots of documentation
 * allow me to replace SCSI HDDs in my legacy Macs with SD cards

## What it's **not**:
 * complete SCSI solution
 * super-fast
 * feature rich


## Development Target
Using the NUCLEO-F401RE board, based around the STM32F401RE. 
This will be fine for development, but is an expensive micro. 
The STM32F401RCT6 is almost as capable and quite a bit cheaper (~$1.80 USD).

## Other projects
If you need a solution today there are a couple of great options:
 
 * SCSI2SD - http://www.codesrc.com/mediawiki/index.php/SCSI2SD

   Feature filled, high-speed and readily available. 
   
 * RaSCSI - http://www.geocities.jp/kugimoto0715/rascsi/
 
   Original built for X68000 machines, this one is based on the Raspberry PI as the host. 
   Looks like it supports Target and Initiator modes. 
   
 

#include "StdAfx.h"
#include "TexUtil.h"

static void Prepare24BitTgaForExport (
   TgaFile *tgaFile, 
   RGBTripple *image,
   int width, 
   int depth
)
{
   tgaFile->header.idLength = 0;
   tgaFile->header.colorMapType = 0;
   tgaFile->header.imageType = UNCOMPRESSED;
   tgaFile->header.colorMapIndex = 0;
   tgaFile->header.colorMapLength = 0;
   tgaFile->header.colorMapEntrySize = 0;
   tgaFile->header.xOrigin = 0;
   tgaFile->header.yOrigin = 0;
   tgaFile->header.width = width;
   tgaFile->header.depth = depth;
   tgaFile->header.bits = 24;
   tgaFile->header.imageDescriptor = 0;
   tgaFile->image.bits24 = image;
}

static void Prepare32BitTgaForExport (
   TgaFile *tgaFile, 
   RGBQuad *image,
   int width, 
   int depth
)
{
   tgaFile->header.idLength = 0;
   tgaFile->header.colorMapType = 0;
   tgaFile->header.imageType = UNCOMPRESSED;
   tgaFile->header.colorMapIndex = 0;
   tgaFile->header.colorMapLength = 0;
   tgaFile->header.colorMapEntrySize = 0;
   tgaFile->header.xOrigin = 0;
   tgaFile->header.yOrigin = 0;
   tgaFile->header.width = width;
   tgaFile->header.depth = depth;
   tgaFile->header.bits = 32;
   tgaFile->header.imageDescriptor = 0;
   tgaFile->image.bits32 = image;
}

int ExportImage32 (
   RGBQuad *image,
   int width, 
   int depth,
   int pitch,
   const char *filename,
   BYTE imageDescriptor
)
{
   TgaFile tgaFile;

   Prepare32BitTgaForExport (&tgaFile, image, width, depth);
   strcpy (tgaFile.fileName, filename);
   return ExportTgaFile (&tgaFile, pitch, 0, imageDescriptor);
}

int ExportImage24 (
   RGBTripple *image,
   int width, 
   int depth,
   int pitch,
   const char *filename,
   BYTE imageDescriptor
)
{
   TgaFile tgaFile;

   Prepare24BitTgaForExport (&tgaFile, image, width, depth);
   strcpy (tgaFile.fileName, filename);
   return ExportTgaFile (&tgaFile, pitch, 0, imageDescriptor);
}

int ExportTgaFile (
   TgaFile *tgaFile,
   int pitch,
   int grnMask,          // only relevant for 15 or 16 bit images
   BYTE imageDescriptor
)
{
   FILE* fp = NULL;
   int i;
   int j;
   int imageSize;

   if ((fp = fopen (tgaFile->fileName,"wb")) == NULL) {
      /*
         cannot open tgafile 
      */
      return 0;
   }

   tgaFile->header.imageDescriptor = imageDescriptor;

   /*
      and uncompressed
   */
   tgaFile->header.imageType = UNCOMPRESSED;

   /* write out the header */
   if (fwrite(&(tgaFile->header.idLength), sizeof(BYTE), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.colorMapType), sizeof(BYTE), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.imageType), sizeof(BYTE), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.colorMapIndex), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.colorMapLength), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.colorMapEntrySize), sizeof(BYTE), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.xOrigin), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.yOrigin), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.width), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.depth), sizeof(short), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.bits), sizeof(BYTE), 1, fp) != 1 ||
       fwrite(&(tgaFile->header.imageDescriptor), sizeof(BYTE), 1, fp) != 1) {
      goto abort;
   }

   imageSize = tgaFile->header.width * tgaFile->header.depth;

   /* write all the data */
   if (tgaFile->header.bits == 24) {
      RGBTripple *image;
      if (pitch == 0) pitch = tgaFile->header.width * sizeof(RGBTripple);
      pitch /= sizeof(RGBTripple);

      for (i = 0; i < tgaFile->header.depth; i++) {
         image = tgaFile->image.bits24 + i * pitch;
         for (j = 0; j < tgaFile->header.width; j++) {
            RGBTripple pixel;
            pixel = *(image + j);
            SWAP_RED_AND_BLU(&pixel);
            if (fwrite(&pixel, sizeof(RGBTripple), 1, fp) != (size_t)1) {
               goto abort;
            }
         }
      }
   }
   else if (tgaFile->header.bits == 32) {
      RGBQuad *image;
      if (pitch == 0) pitch = tgaFile->header.width * sizeof(RGBQuad);
      pitch /= sizeof(RGBQuad);

#ifdef PC
      /*
         Red and Blue should be swapped for the PC in an 8888 case
      */
      for (i = 0; i < tgaFile->header.depth; i++) {
         image = tgaFile->image.bits32 + i * pitch;
         for (j = 0; j < tgaFile->header.width; j++) {
            RGBQuad pixel;
            pixel = *(image + j);
            SWAP_RED_AND_BLU(&pixel);
            *(image + j) = pixel;
         }
      }
#endif

      int size = tgaFile->header.width * tgaFile->header.depth;
      image = tgaFile->image.bits32;
		// debugged by chaen
      int rowLength = tgaFile->header.width;
      for (i = 0; i < size; i+=rowLength) {
         if (fwrite(image, sizeof(RGBQuad), rowLength, fp) != (size_t)rowLength) {
            goto abort;
         }
         image+=pitch;
      }
		// Older..
//       int rowLength = tgaFile->header.width * pitch;
//       for (i = 0; i < size; i+=rowLength) {
//          if (fwrite(image, sizeof(RGBQuad), rowLength, fp) != (size_t)rowLength) {
//             goto abort;
//          }
//          image+=rowLength;
//       }
   }
   else {
      RGBTripple15 *image;
      RGBTripple15 pixel;
      if (pitch == 0) pitch = tgaFile->header.width * sizeof(RGBTripple15);
      pitch /= sizeof(RGBTripple15);

      if (grnMask == (31 << 5)) {
         /*
            15 bit source data, just write it out
         */
         for (i = 0; i < tgaFile->header.depth; i++) {
            image = tgaFile->image.bits16 + i * pitch;
            if (fwrite (image, sizeof(RGBTripple15), tgaFile->header.width, fp) != (unsigned)tgaFile->header.width) return 0;
         }
      }
      else {
         /*
            16 bit source data, reduce to 15 bit and write it out
         */
         for (i = 0; i < tgaFile->header.depth; i++) {
            image = tgaFile->image.bits16 + i * pitch;
            for (j = 0; j < tgaFile->header.width; j++) {
               pixel = (*image & 31)
                     | (((*image & 0xffe0) >> 1) & 0x7fe0);
               if (fwrite (&pixel, sizeof(RGBTripple15), 1, fp) != 1) return 0;
               image++;
            }
         }
      }
   }
   fclose (fp);
   return 1;

abort:
   if (fp != NULL) fclose(fp);
   return 0;
}

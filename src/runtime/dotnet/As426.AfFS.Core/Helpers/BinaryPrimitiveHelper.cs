using System.Buffers.Binary; // This is a key library in modern .NET
using System.Text;

namespace As426.AfFS.Core.Helpers
{
   /// <summary>
   /// Helper methods for working with binary data according to the AfFS Binary Layout v1 specification [2][5].
   /// Ensures Little-Endian encoding and checksums.
   /// </summary>
   public static class BinaryPrimitivesHelper
   {
      // --- Specification Constants ---
      public const int SizeOfInt32 = 4;
      public const int SizeOfInt64 = 8;
      public const int SizeOfGuid = 16;

      // --- 1. Endianness (Little-Endian) ---
      // These methods take a number and store it in the buffer at the correct position.
      // Or they load a number from the buffer.

      public static void WriteInt16LittleEndian(byte[] buffer, int offset, short value)
      {
         BinaryPrimitives.WriteInt16LittleEndian(buffer.AsSpan(offset, 2), value);
      }

      public static short ReadInt16LittleEndian(byte[] buffer, int offset)
      {
         return BinaryPrimitives.ReadInt16LittleEndian(buffer.AsSpan(offset, 2));
      }

      public static void WriteInt32LittleEndian(byte[] buffer, int offset, int value)
      {
         BinaryPrimitives.WriteInt32LittleEndian(buffer.AsSpan(offset, 4), value);
      }

      public static int ReadInt32LittleEndian(byte[] buffer, int offset)
      {
         return BinaryPrimitives.ReadInt32LittleEndian(buffer.AsSpan(offset, 4));
      }

      public static void WriteInt64LittleEndian(byte[] buffer, int offset, long value)
      {
         BinaryPrimitives.WriteInt64LittleEndian(buffer.AsSpan(offset, 8), value);
      }

      public static long ReadInt64LittleEndian(byte[] buffer, int offset)
      {
         return BinaryPrimitives.ReadInt64LittleEndian(buffer.AsSpan(offset, 8));
      }

      // --- 2. Checksum (CRC32C) ---
      // According to the documentation, the default checksum is CRC32C [2].

      /// <summary>
      /// Calculates the CRC32C hash for the specified data.
      /// Replaced with custom implementation because System.IO.Hashing is not available.
      /// </summary>
      public static uint ComputeCrc32C(byte[] data)
      {
         // .NET does not have built-in CRC32C, so we use a Managed implementation (e.g., from a third party or custom).
         // Here is a simple CRC32C (Castagnoli) implementation for .NET.
         const uint Polynomial = 0x82F63B78u;
         uint crc = 0xFFFFFFFFu;

         foreach(byte b in data)
         {
            crc ^= b;
            for(int i = 0; i < 8; i++)
            {
               if((crc & 1) != 0)
                  crc = (crc >> 1) ^ Polynomial;
               else
                  crc >>= 1;
            }
         }
         return ~crc;
      }

      // --- 3. Helper methods for text and magic numbers ---

      /// <summary>
      /// Writes a "Magic Number" (ASCII text) to the buffer.
      /// e.g. 'AFR1' -> [65][70][82][49]
      /// </summary>
      public static void WriteMagic(byte[] buffer, int offset, string magic)
      {
         byte[] magicBytes = Encoding.ASCII.GetBytes(magic);
         Array.Copy(magicBytes, 0, buffer, offset, magicBytes.Length);
      }

      /// <summary>
      /// Loads the Magic Number and returns it as text for verification.
      /// </summary>
      public static string ReadMagic(byte[] buffer, int offset, int length)
      {
         return Encoding.ASCII.GetString(buffer, offset, length);
      }

      public static void WriteUInt16LittleEndian(byte[] buffer, int offset, ushort value)
      {
         BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset, 2), value);
      }

      // And for reading:
      public static ushort ReadUInt16LittleEndian(byte[] buffer, int offset)
      {
         return BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset, 2));
      }
   }
}
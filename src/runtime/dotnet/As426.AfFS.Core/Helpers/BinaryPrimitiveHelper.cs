using System.Buffers.Binary; // Tohle je klíčová knihovna v moderním .NET
using System.Text;

namespace As426.AfFS.Core.Helpers
{
   /// <summary>
   /// Pomocné metody pro práci s binárními daty dle specifikace AfFS Binary Layout v1 [2][5].
   /// Zajišťuje Little-Endian kódování a kontrolní součty.
   /// </summary>
   public static class BinaryPrimitivesHelper
   {
      // --- Konstanty dle specifikace ---
      public const int SizeOfInt32 = 4;
      public const int SizeOfInt64 = 8;
      public const int SizeOfGuid = 16;

      // --- 1. Endianness (Little-Endian) ---
      // Tyto metody berou číslo a uloží ho do bufferu na správné místo.
      // Nebo načítají číslo z bufferu.

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

      // --- 2. Kontrolní součet (CRC32C) ---
      // Dle dokumentace je defaultní checksum CRC32C [2].

      /// <summary>
      /// Vypočítá CRC32C hash pro zadaná data.
      /// Nahrazeno vlastní implementací, protože System.IO.Hashing není dostupné.
      /// </summary>
      public static uint ComputeCrc32C(byte[] data)
      {
         // .NET nemá vestavěnou CRC32C, použijeme Managed implementaci (např. od třetí strany nebo vlastní).
         // Zde je jednoduchá implementace CRC32C (Castagnoli) pro .NET.
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

      // --- 3. Pomocné metody pro text a magická čísla ---

      /// <summary>
      /// Zapíše "Magic Number" (ASCII text) do bufferu.
      /// Např. 'AFR1' -> [65][70][82][49]
      /// </summary>
      public static void WriteMagic(byte[] buffer, int offset, string magic)
      {
         byte[] magicBytes = Encoding.ASCII.GetBytes(magic);
         Array.Copy(magicBytes, 0, buffer, offset, magicBytes.Length);
      }

      /// <summary>
      /// Načte Magic Number a vrátí ho jako text pro kontrolu.
      /// </summary>
      public static string ReadMagic(byte[] buffer, int offset, int length)
      {
         return Encoding.ASCII.GetString(buffer, offset, length);
      }

      public static void WriteUInt16LittleEndian(byte[] buffer, int offset, ushort value)
      {
         BinaryPrimitives.WriteUInt16LittleEndian(buffer.AsSpan(offset, 2), value);
      }

      // A pro čtení:
      public static ushort ReadUInt16LittleEndian(byte[] buffer, int offset)
      {
         return BinaryPrimitives.ReadUInt16LittleEndian(buffer.AsSpan(offset, 2));
      }
   }
}
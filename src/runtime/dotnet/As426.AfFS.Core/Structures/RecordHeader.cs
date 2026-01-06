using As426.AfFS.Core.Helpers;

namespace As426.AfFS.Core.Structures
{
   /// <summary>
   /// Hlavička záznamu AfFS (Record Framing).
   /// Velikost je 22 bajtů [2].
   /// Definuje, co data jsou, jak jsou velká a jak jsou chráněna.
   /// </summary>
   public struct RecordHeader
   {
      // --- Konstanty z dokumentace ---
      public const byte LayoutVersion = 1;
      public const string MagicString = "AFR1";

      // --- Políčka ---
      // Celkem: 4 (Magic) + 1 (Version) + 1 (Kind) + 1 (SizeClass) + 1 (AccessProfile) + 2 (Flags) + 8 (PayloadLength) + 4 (Checksum) = 22 bajtů

      public RecordKind Kind { get; set; } // Byte 4
      public byte SizeClass { get; set; }  // Byte 5
      public byte AccessProfile { get; set; } // Byte 6
      public ushort Flags { get; set; }    // Bytes 7-8
      public long PayloadLength { get; set; } // Bytes 9-16
      public uint Checksum { get; set; }   // Bytes 17-20

      /// <summary>
      /// Celková velikost hlavičky v bajtech.
      /// </summary>
      public const int HeaderSize = 22;

      /// <summary>
      /// Serializuje hlavičku do pole bajtů.
      /// Pozor: Checksum vypočítáme až budeme mít payload. Zde ho jen zapíšeme.
      /// </summary>
      public byte[] ToBytes()
      {
         byte[] buffer = new byte[HeaderSize];

         // 1. Magic (4 bajty): 'AFR1'
         BinaryPrimitivesHelper.WriteMagic(buffer, 0, MagicString);

         // 2. Version (1 bajt)
         buffer[4] = LayoutVersion;

         // 3. Kind, SizeClass, AccessProfile, Flags
         buffer[5] = (byte)Kind;
         buffer[6] = SizeClass;
         buffer[7] = AccessProfile;
         // Oprava: Přetypování Flags z ushort na short při volání WriteInt16LittleEndian
         BinaryPrimitivesHelper.WriteInt16LittleEndian(buffer, 8, (short)Flags);

         // 4. PayloadLength (8 bajtů)
         BinaryPrimitivesHelper.WriteInt64LittleEndian(buffer, 10, PayloadLength);

         // 5. Checksum (4 bajtů) - Offset 18
         BinaryPrimitivesHelper.WriteInt32LittleEndian(buffer, 18, (int)Checksum);

         return buffer;
      }

      /// <summary>
      /// Deserializuje hlavičku z bajtů.
      /// Vrátí true, pokud se zdá být validní (magic sedí), jinak false.
      /// </summary>
      public bool TryParse(byte[] buffer)
      {
         if(buffer == null || buffer.Length < HeaderSize) return false;

         // 1. Kontrola Magic (první 4 bajty)
         string magic = BinaryPrimitivesHelper.ReadMagic(buffer, 0, 4);
         if(magic != MagicString) return false;

         // 2. Kontrola Version
         byte version = buffer[4];
         if(version != LayoutVersion) return false;

         // 3. Načtení dat
         Kind = (RecordKind)buffer[5];
         SizeClass = buffer[6];
         AccessProfile = buffer[7];
         Flags = (ushort)BinaryPrimitivesHelper.ReadInt16LittleEndian(buffer, 8);
         PayloadLength = BinaryPrimitivesHelper.ReadInt64LittleEndian(buffer, 10);
         Checksum = (uint)BinaryPrimitivesHelper.ReadInt32LittleEndian(buffer, 18);

         return true;
      }
   }
}
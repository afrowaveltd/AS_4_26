using As426.AfFS.Core.Helpers;

namespace As426.AfFS.Core.Structures
{
   /// <summary>
   /// AfFS Record Header (Record Framing).
   /// Size is 22 bytes [2].
   /// Defines what the data is, how large it is, and how it is protected.
   /// </summary>
   public struct RecordHeader
   {
      // --- Documentation Constants ---
      public const byte LayoutVersion = 1;
      public const string MagicString = "AFR1";

      // --- Fields ---
      // Total: 4 (Magic) + 1 (Version) + 1 (Kind) + 1 (SizeClass) + 1 (AccessProfile) + 2 (Flags) + 8 (PayloadLength) + 4 (Checksum) = 22 bytes

      public RecordKind Kind { get; set; } // Byte 4
      public byte SizeClass { get; set; }  // Byte 5
      public byte AccessProfile { get; set; } // Byte 6
      public ushort Flags { get; set; }    // Bytes 7-8
      public long PayloadLength { get; set; } // Bytes 9-16
      public uint Checksum { get; set; }   // Bytes 17-20

      /// <summary>
      /// Total size of the header in bytes.
      /// </summary>
      public const int HeaderSize = 22;

      /// <summary>
      /// Serializes the header into a byte array.
      /// Note: The checksum is calculated only once the payload is available. It is only written here.
      /// </summary>
      public byte[] ToBytes()
      {
         byte[] buffer = new byte[HeaderSize];

         // 1. Magic (4 bytes): 'AFR1'
         BinaryPrimitivesHelper.WriteMagic(buffer, 0, MagicString);

         // 2. Version (1 byte)
         buffer[4] = LayoutVersion;

         // 3. Kind, SizeClass, AccessProfile, Flags
         buffer[5] = (byte)Kind;
         buffer[6] = SizeClass;
         buffer[7] = AccessProfile;
         // Fix: Casting Flags from ushort to short when calling WriteInt16LittleEndian
         BinaryPrimitivesHelper.WriteInt16LittleEndian(buffer, 8, (short)Flags);

         // 4. PayloadLength (8 bytes)
         BinaryPrimitivesHelper.WriteInt64LittleEndian(buffer, 10, PayloadLength);

         // 5. Checksum (4 bytes) - Offset 18
         BinaryPrimitivesHelper.WriteInt32LittleEndian(buffer, 18, (int)Checksum);

         return buffer;
      }

      /// <summary>
      /// Deserializes the header from bytes.
      /// Returns true if it appears valid (magic matches), otherwise false.
      /// </summary>
      public bool TryParse(byte[] buffer)
      {
         if(buffer == null || buffer.Length < HeaderSize) return false;

         // 1. Magic check (first 4 bytes)
         string magic = BinaryPrimitivesHelper.ReadMagic(buffer, 0, 4);
         if(magic != MagicString) return false;

         // 2. Version check
         byte version = buffer[4];
         if(version != LayoutVersion) return false;

         // 3. Data loading
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
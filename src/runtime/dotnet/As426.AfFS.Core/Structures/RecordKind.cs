namespace As426.AfFS.Core.Structures
{
   /// <summary>
   /// Data Kind defines the semantic role of the record [2].
   /// (Simplified for M3 - more types will be added over time)
   /// </summary>
   public enum RecordKind : byte
   {
      Uninitialized = 0,
      StructuralData = 1,   // Schema, metadata
      Scalar = 2,           // Atomic values
      Row = 3,              // Table rows
      Text = 4,             // Text
      Blob = 5,             // Binary data
      Log = 6               // Logs
   }
}
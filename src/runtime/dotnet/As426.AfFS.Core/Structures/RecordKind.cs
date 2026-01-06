namespace As426.AfFS.Core.Structures
{
   /// <summary>
   /// Data Kind definuje sémantickou roli záznamu [2].
   /// (Zjednodušeno pro M3 - časem přidáme více typů)
   /// </summary>
   public enum RecordKind : byte
   {
      Uninitialized = 0,
      StructuralData = 1,   // Schéma, metadata
      Scalar = 2,           // Atomické hodnoty
      Row = 3,              // Řádky tabulek
      Text = 4,             // Text
      Blob = 5,             // Binární data
      Log = 6               // Logy
   }
}
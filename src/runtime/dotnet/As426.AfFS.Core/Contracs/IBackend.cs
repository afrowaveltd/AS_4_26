namespace As426.AfFS.Core.Contracts
{
   /// <summary>
   /// Kategorizuje výsledek operace, jak je definováno v "Backend I/O Layer" [2].
   /// Runtime používá tyto kategorie k deterministickému rozhodování.
   /// </summary>
   public enum BackendOperationCategory
   {
      Ok,             // Operace proběhla v pořádku.
      Partial,        // Některé bajty byly zpracovány, poté došlo k chybě.
      TransientError, // Dočasná chyba (např. timeout, přetížení), lze zkusit znovu.
      PermanentError, // Trvalá chyba média (např. bad sector), opakování je zbytečné.
      Timeout,        // Časový limit operace byl překročen.
      AccessDenied,   // Chyba oprávnění nebo přístupu.
      OutOfSpace      // Diskové místo je vyčerpáno.
   }

   /// <summary>
   /// Strukturovaný výsledek operace. Neslouží jako výjimka, ale jako hodnota [1][4].
   /// Umožňuje vrátit podrobnosti o tom, co se skutečně stalo (např. BytesProcessed).
   /// </summary>
   public struct BackendResult
   {
      public BackendOperationCategory Status { get; set; }
      public long BytesProcessed { get; set; }
      public int? OsErrorCode { get; set; }

      public override string ToString()
      {
         return $"Status: {Status}, Bytes: {BytesProcessed}, OS Error: {OsErrorCode?.ToString() ?? "None"}";
      }
   }

   /// <summary>
   /// Abstraktní vrstva pro I/O operace. Odděluje logiku Runtime od fyzického úložiště [2].
   /// Musí být implementovatelná jak v .NET (FileStream), tak v C (BIOS/Block device) pro BOS.
   /// </summary>
   public interface IBackend
   {
      /// <summary>
      /// Čte data z určitého offsetu do poskytnutého bufferu.
      /// </summary>
      /// <param name="offset">Absolutní pozice v bajtech.</param>
      /// <param name="buffer">Buffer pro zapsání přečtených dat.</param>
      /// <param name="count">Počet bajtů, které chceme přečíst.</param>
      /// <returns>BackendResult s počtem přečtených bajtů a statusem.</returns>
      BackendResult Read(long offset, byte[] buffer, int count);

      /// <summary>
      /// Zapisuje data na určitý offset.
      /// </summary>
      /// <param name="offset">Absolutní pozice v bajtech.</param>
      /// <param name="buffer">Buffer s daty k zápisu.</param>
      /// <returns>BackendResult s počtem zapsaných bajtů a statusem.</returns>
      BackendResult Write(long offset, byte[] buffer, int count);

      /// <summary>
      /// Zajišťuje trvanlivost dat (durability barrier).
      /// Vynutí zápis cache disku do fyzického média [2].
      /// </summary>
      BackendResult Flush();

      /// <summary>
      /// Vrací celkovou velikost úložiště v bajtech.
      /// </summary>
      long GetSize();

      /// <summary>
      /// Vrací preferované zarovnání pro operace (např. velikost sektoru).
      /// Důležité pro efektivní I/O a zamezení nezarovnaných čtení [2].
      /// </summary>
      int GetPreferredAlignment();
   }
}
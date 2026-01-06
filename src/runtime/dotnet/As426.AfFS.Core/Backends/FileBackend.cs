using As426.AfFS.Core.Contracts;

namespace As426.AfFS.Core.Backends
{
   /// <summary>
   /// Implementace Backend I/O vrstvy pomocí standardního souboru na hostitelském systému.
   /// Implementuje kontrakt IBackend a mapuje I/O výjimky na strukturované BackendResult [2][1].
   /// </summary>
   public class FileBackend : IBackend
   {
      private readonly string _filePath;
      private readonly FileStream _stream;

      public FileBackend(string filePath, FileMode mode, FileAccess access)
      {
         _filePath = filePath;
         // Stream držíme otevřený po celou dobu života backendu.
         _stream = new FileStream(filePath, mode, access, FileShare.None);
      }

      /// <summary>
      /// Přečte bajty z offsetu. Vrací BackendResult s počtem přečtených bajtů.
      /// </summary>
      public BackendResult Read(long offset, byte[] buffer, int count)
      {
         try
         {
            // Ujistíme se, že jsme na správné pozici (explicitní offset)
            _stream.Seek(offset, SeekOrigin.Begin);

            // FileStream.Read vrací počet přečtených bajtů (může být 0, pokud je konec souboru)
            int bytesRead = _stream.Read(buffer, 0, count);

            return new BackendResult
            {
               Status = BackendOperationCategory.Ok,
               BytesProcessed = bytesRead
            };
         }
         catch(IOException ioEx)
         {
            // I/O výjimky v .NET jsou často dočasné (uzamčení souboru, chyba sítě) nebo trvalé (chyba disku).
            // Zjednodušeně je označujeme jako TransientError, pokud specificky neidentifikujeme poškozený sektor.
            return new BackendResult
            {
               Status = BackendOperationCategory.TransientError,
               BytesProcessed = 0,
               OsErrorCode = 0 // Nelze snadno získat Win32/Linux error code bez P/Invoke
            };
         }
         catch(Exception ex)
         {
            // Vše ostatní bereme jako AccessDenied nebo obecnou chybu
            return new BackendResult
            {
               Status = BackendOperationCategory.AccessDenied,
               BytesProcessed = 0
            };
         }
      }

      /// <summary>
      /// Zapíše bajty na offset.
      /// </summary>
      public BackendResult Write(long offset, byte[] buffer, int count)
      {
         try
         {
            _stream.Seek(offset, SeekOrigin.Begin);
            _stream.Write(buffer, 0, count);

            return new BackendResult
            {
               Status = BackendOperationCategory.Ok,
               BytesProcessed = count
            };
         }
         catch(IOException)
         {
            return new BackendResult
            {
               Status = BackendOperationCategory.TransientError,
               BytesProcessed = 0
            };
         }
         catch(Exception)
         {
            return new BackendResult
            {
               Status = BackendOperationCategory.AccessDenied,
               BytesProcessed = 0
            };
         }
      }

      /// <summary>
      /// Zajišťuje, že bajty jsou fyzicky na disku (Durability Barrier) [2].
      /// </summary>
      public BackendResult Flush()
      {
         try
         {
            _stream.Flush(true); // true = flush to disk, ne jen do OS cache
            return new BackendResult
            {
               Status = BackendOperationCategory.Ok,
               BytesProcessed = 0
            };
         }
         catch(Exception)
         {
            return new BackendResult
            {
               Status = BackendOperationCategory.TransientError,
               BytesProcessed = 0
            };
         }
      }

      /// <summary>
      /// Vrací délku souboru (velikost úložiště).
      /// </summary>
      public long GetSize()
      {
         return _stream.Length;
      }

      /// <summary>
      /// Pro soubor na filesystému vrátíme 1 bajt (není tam žádná bloková překážka).
      /// Pro bloková zařízení to bude velikost sektoru [2].
      /// </summary>
      public int GetPreferredAlignment()
      {
         return 1;
      }

      // Důležité pro uvolnění zdrojů (implementace IDisposable by byla lepší, ale pro MVP stačí Dispose metoda)
      public void Dispose()
      {
         _stream?.Dispose();
      }
   }
}
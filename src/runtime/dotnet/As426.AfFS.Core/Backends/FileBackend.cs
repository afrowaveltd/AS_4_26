using As426.AfFS.Core.Contracts;

namespace As426.AfFS.Core.Backends
{
   /// <summary>
   /// Implementation of the Backend I/O layer using a standard file on the host system.
   /// Implements the IBackend contract and maps I/O exceptions to structured BackendResults [2][1].
   /// </summary>
   public class FileBackend : IBackend
   {
      private readonly string _filePath;
      private readonly FileStream _stream;

      public FileBackend(string filePath, FileMode mode, FileAccess access)
      {
         _filePath = filePath;
         // The stream is kept open for the lifetime of the backend.
         _stream = new FileStream(filePath, mode, access, FileShare.None);
      }

      /// <summary>
      /// Reads bytes from an offset. Returns a BackendResult with the number of bytes read.
      /// </summary>
      public BackendResult Read(long offset, byte[] buffer, int count)
      {
         try
         {
            // Ensuring we are at the correct position (explicit offset)
            _stream.Seek(offset, SeekOrigin.Begin);

            // FileStream.Read returns the number of bytes read (can be 0 if end of file)
            int bytesRead = _stream.Read(buffer, 0, count);

            return new BackendResult
            {
               Status = BackendOperationCategory.Ok,
               BytesProcessed = bytesRead
            };
         }
         catch(IOException ioEx)
         {
            // I/O exceptions in .NET are often transient (file lock, network error) or permanent (disk error).
            // Simplistically, we label them as TransientError unless a corrupted sector is specifically identified.
            return new BackendResult
            {
               Status = BackendOperationCategory.TransientError,
               BytesProcessed = 0,
               OsErrorCode = 0 // Win32/Linux error codes cannot be easily obtained without P/Invoke
            };
         }
         catch(Exception ex)
         {
            // Everything else is treated as AccessDenied or a general error
            return new BackendResult
            {
               Status = BackendOperationCategory.AccessDenied,
               BytesProcessed = 0
            };
         }
      }

      /// <summary>
      /// Writes bytes to an offset.
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
      /// Ensures that bytes are physically on disk (Durability Barrier) [2].
      /// </summary>
      public BackendResult Flush()
      {
         try
         {
            _stream.Flush(true); // true = flush to disk, not just OS cache
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
      /// Returns file length (storage size).
      /// </summary>
      public long GetSize()
      {
         return _stream.Length;
      }

      /// <summary>
      /// For a file on the filesystem, we return 1 byte (no block boundary).
      /// For block devices, this would be the sector size [2].
      /// </summary>
      public int GetPreferredAlignment()
      {
         return 1;
      }

      // Important for releasing resources (IDisposable implementation would be better, but a Dispose method suffices for MVP)
      public void Dispose()
      {
         _stream?.Dispose();
      }
   }
}
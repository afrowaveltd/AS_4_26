namespace As426.AfFS.Core.Contracts
{
   /// <summary>
   /// Categorizes the operation result as defined in "Backend I/O Layer" [2].
   /// The runtime uses these categories for deterministic decision-making.
   /// </summary>
   public enum BackendOperationCategory
   {
      Ok,             // Operation was successful.
      Partial,        // Some bytes were processed, then an error occurred.
      TransientError, // Transient error (e.g., timeout, overload), can be retried.
      PermanentError, // Permanent media error (e.g., bad sector), repeating is useless.
      Timeout,        // Operation timeout exceeded.
      AccessDenied,   // Permission or access error.
      OutOfSpace      // Disk space exhausted.
   }

   /// <summary>
   /// Structured operation result. It does not serve as an exception, but as a value [1][4].
   /// Allows returning details about what actually happened (e.g., BytesProcessed).
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
   /// Abstract layer for I/O operations. Separates runtime logic from physical storage [2].
   /// Must be implementable in both .NET (FileStream) and C (BIOS/Block device) for BOS.
   /// </summary>
   public interface IBackend
   {
      /// <summary>
      /// Reads data from a specific offset into the provided buffer.
      /// </summary>
      /// <param name="offset">Absolute position in bytes.</param>
      /// <param name="buffer">Buffer for writing the read data.</param>
      /// <param name="count">Number of bytes we want to read.</param>
      /// <returns>BackendResult with the number of bytes read and status.</returns>
      BackendResult Read(long offset, byte[] buffer, int count);

      /// <summary>
      /// Writes data to a specific offset.
      /// </summary>
      /// <param name="offset">Absolute position in bytes.</param>
      /// <param name="buffer">Buffer with data for writing.</param>
      /// <returns>BackendResult with the number of bytes written and status.</returns>
      BackendResult Write(long offset, byte[] buffer, int count);

      /// <summary>
      /// Ensures data durability (durability barrier).
      /// Forces disk cache write to physical media [2].
      /// </summary>
      BackendResult Flush();

      /// <summary>
      /// Returns total storage size in bytes.
      /// </summary>
      long GetSize();

      /// <summary>
      /// Returns preferred alignment for operations (e.g., sector size).
      /// Important for efficient I/O and prevention of unaligned reads [2].
      /// </summary>
      int GetPreferredAlignment();
   }
}
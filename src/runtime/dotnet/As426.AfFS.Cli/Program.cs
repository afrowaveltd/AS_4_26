using As426.AfFS.Core.Backends;
using System.Text;

namespace As426.AfFS.Cli
{
   class Program
   {
      static void Main(string[] args)
      {
         Console.WriteLine("=== AfFS M1: Backend I/O Test ===");

         // 1. Creating FileBackend (creates file if it doesn't exist)
         Console.WriteLine("[Init] Creating backend for file 'test-store.afs'...");
         var backend = new FileBackend("test-store.afs", FileMode.OpenOrCreate, FileAccess.ReadWrite);

         // 2. Preparing data - what we want to store
         string message = "Hello world from position 100!";
         byte[] dataToWrite = Encoding.UTF8.GetBytes(message);

         // 3. WRITE: Writing to explicit offset 100
         Console.WriteLine($"[Write] Writing '{message}' to offset 100...");
         var writeResult = backend.Write(100, dataToWrite, dataToWrite.Length);

         Console.WriteLine($"  -> Result: {writeResult}");

         // 4. DURABILITY: Ensure it's on disk
         Console.WriteLine("[Flush] Syncing write to physical disk...");
         var flushResult = backend.Flush();
         Console.WriteLine($"  -> Result: {flushResult}");

         // 5. READ: Reading back
         byte[] readBuffer = new byte[dataToWrite.Length]; // Buffer of same size
         Console.WriteLine("[Read] Reading data from offset 100...");
         var readResult = backend.Read(100, readBuffer, readBuffer.Length);

         Console.WriteLine($"  -> Result: {readResult}");

         // 6. Decoding and check
         string readMessage = Encoding.UTF8.GetString(readBuffer);
         Console.WriteLine($"[Check] Read message: '{readMessage}'");

         if(readMessage == message)
         {
            Console.WriteLine("-> SUCCESS: Message matches. Backend is working!");
         }
         else
         {
            Console.WriteLine("-> ERROR: Message mismatch.");
         }

         // 7. Cleanup
         backend.Dispose();
         Console.WriteLine("\nPress any key to exit...");
         Console.ReadKey();
      }
   }
}
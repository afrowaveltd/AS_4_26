using As426.AfFS.Core.Backends;
using System.Text;

namespace As426.AfFS.Cli
{
   class Program
   {
      static void Main(string[] args)
      {
         Console.WriteLine("=== AfFS M1: Backend I/O Test ===");

         // 1. Vytvoření FileBackendu (založí soubor, pokud neexistuje)
         Console.WriteLine("[Init] Vytvářím backend pro soubor 'test-store.afs'...");
         var backend = new FileBackend("test-store.afs", FileMode.OpenOrCreate, FileAccess.ReadWrite);

         // 2. Příprava dat - co chceme uložit
         string message = "Ahoj světe z pozice 100!";
         byte[] dataToWrite = Encoding.UTF8.GetBytes(message);

         // 3. ZÁPIS: Píšeme na explicitní offset 100
         Console.WriteLine($"[Write] Zapisuji '{message}' na offset 100...");
         var writeResult = backend.Write(100, dataToWrite, dataToWrite.Length);

         Console.WriteLine($"  -> Výsledek: {writeResult}");

         // 4. DURABILITY: Zajistíme, že to je na disku
         Console.WriteLine("[Flush] Zamykám zápis na fyzický disk...");
         var flushResult = backend.Flush();
         Console.WriteLine($"  -> Výsledek: {flushResult}");

         // 5. ČTENÍ: Přečteme zpět
         byte[] readBuffer = new byte[dataToWrite.Length]; // Buffer stejné velikosti
         Console.WriteLine("[Read] Čtu data ze offsetu 100...");
         var readResult = backend.Read(100, readBuffer, readBuffer.Length);

         Console.WriteLine($"  -> Výsledek: {readResult}");

         // 6. Dekódování a kontrola
         string readMessage = Encoding.UTF8.GetString(readBuffer);
         Console.WriteLine($"[Check] Přečtená zpráva: '{readMessage}'");

         if(readMessage == message)
         {
            Console.WriteLine("-> ÚSPĚCH: Zpráva sedí. Backend funguje!");
         }
         else
         {
            Console.WriteLine("-> CHYBA: Zpráva nesedí.");
         }

         // 7. Úklid
         backend.Dispose();
         Console.WriteLine("\nStiskni klávesu pro ukončení...");
         Console.ReadKey();
      }
   }
}
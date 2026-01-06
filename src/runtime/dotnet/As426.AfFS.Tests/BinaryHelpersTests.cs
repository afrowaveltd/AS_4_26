using As426.AfFS.Core.Helpers;
using System.Text;

namespace As426.AfFS.Tests
{
   public class BinaryHelpersTests
   {
      [Fact]
      public void ComputeCrc32C_ShouldMatchKnownVector()
      {
         // Arrange
         // Standardní testovací řetězec pro CRC32C
         byte[] data = Encoding.ASCII.GetBytes("123456789");

         // Expected value pro Castagnoli polynom
         uint expectedChecksum = 0xE3069283u;

         // Act
         uint actualChecksum = BinaryPrimitivesHelper.ComputeCrc32C(data);

         // Assert
         Assert.Equal(expectedChecksum, actualChecksum);

         // Pro kontrolu vypíšeme do konzole
         Console.WriteLine($"Expected: 0x{expectedChecksum:X8}");
         Console.WriteLine($"Actual:   0x{actualChecksum:X8}");
      }
   }
}
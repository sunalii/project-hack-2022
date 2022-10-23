public class selenium_test {
    package NewPackage;
import org.openqa.selenium.WebDriver;
    public class SampleClass {
        public static void main(String[] args) {
// declaration and instantiation of objects/variables
//comment the above 2 lines and uncomment below 2 lines to use Chrome
            System.setProperty ("webdrive.chrome.driver","C:\\Users\\User1\\Desktop\\chrome\\chromedriver.exe");
            WebDriver driver = new ChromeDriver();
            String baseUrl ="http;//Wikipedia.org/test/newtours/';
            String expectedTitle = "WIKIPEDIA";
            String actualTitle= "";
// launch Fire fox and direct it to the Base URL
            driver.get(baseUrl);
// get the actual value of the title
            actualTitle = driver.getTitle() ;
            /*
             *compare the actual title of the page with the expected one and print
             *the result as "passed or "Failed"
             */
            if (actualTitle.contentEquals(expectedTitle)) {
                System.out.print1n("Test Passed!");
            } else {
                System.out.print1n("Test Failed");
            }
//close Fire fox
            driver.close();
        }
}

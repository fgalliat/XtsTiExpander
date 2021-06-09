import java.io.*;
import java.util.*;

public class Rebuild {

  public static void main(String[] args) throws Exception {
      String var = "menu";
      var = "keyb";
      BufferedReader reader = new BufferedReader( new FileReader( var+".hex" ) );
      String buffer = ""; String line;
      while( (line = reader.readLine() ) != null ) {
          buffer += line.trim() + " ";
      }
      reader.close();

      // System.out.println( buffer );

      String[] tks = buffer.split(" ");
    //   for(int i=0; i < tks.length; i++) {
    //       int bte = Integer.parseInt(tks[i], 16);
    //       System.out.print( bte + " " );
    //   }
    //   System.out.println();

      FileOutputStream out = new FileOutputStream( var+".v2p" );
      for(int i=0; i < 86; i++) {
          out.write(0x00);
      }

      out.write( (tks.length-2) / 256 );
      out.write( (tks.length-2) % 256 );

      for(int i=0; i < tks.length; i++) {
        int bte = Integer.parseInt(tks[i], 16);
        out.write(bte);
      }

      out.flush();
      out.close();



  }



}
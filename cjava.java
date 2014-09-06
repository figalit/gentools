import java.util.*;
import java.io.*;

class clique{
    Vector list = new Vector(0);
    public static void main(String args[]){
	String Line = new String("");
	BufferedReader in = new BufferedReader(new FileReader(args[1]));
	StringTokenizer token;
	Integer i1, i2;
	element elem=new element();
	try{
	    while ((Line = in.readLine()) != null){
		token = New StringTokenizer(Line," ",false);
		i1 = new Integer(token.nextToken());
		i2 = new Integer(token.nextToken());
		elem.set = new HashSet();
		elem.set.add(i2);
		
	    } // while
	} catch (Exception ioe){ }
    } // main
} // class clique

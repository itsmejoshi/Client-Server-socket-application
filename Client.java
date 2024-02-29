import java.io.*;
import java.net.*;

/** client.java, 
 * this client class makes a connection to the server, sends commands and handles server responses*/

 public class Client {
    //constant to store port number
    private static final int SERVER_PORT = 5206;
    public static void main(String[] args)
    {
        PrintWriter printWriter = null;
        BufferedReader bufferedReader = null;
        String sAddress = args[0];
        InetAddress host;
        Socket s = null;
       
//validation check the number of command line parameters
        if (args.length < 1) {
            System.out.println("Usage: JAVA Client <Server IP Address>");
            System.exit(1);
        }
//connection setup
        try {
            host = InetAddress.getByName(sAddress);
            s = new Socket(host, SERVER_PORT);
            printWriter = new PrintWriter(s.getOutputStream(), true);
            bufferedReader = new BufferedReader(new InputStreamReader(s.getInputStream()));
        } catch (UnknownHostException e) {
            System.err.println("Unknown host: " + sAddress);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to: " + sAddress);
            System.exit(1);
        }

        System.out.println("Welcome to Client.");

        try (BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))) {
            String userInput;

            while (true) {
                System.out.print("command: ");
                userInput = stdIn.readLine();

                if (userInput == null) {
                    break;
                }

                // Lowercase the input to unify commands
                userInput = userInput.toLowerCase();

                // MSGGET
                if (userInput.equals("msgget")) {
                    printWriter.println(userInput);
                    String response;
                    while ((response = bufferedReader.readLine()) != null && !response.isEmpty()) {
                        System.out.println(response);
                    }
                }

                // QUIT
                else if (userInput.equals("quit")) {
                    printWriter.println(userInput);
                    String response = bufferedReader.readLine();
                    System.out.println(response);
                    if (response.equals("200 OK")) {
                        break;
                    }
                }

                // SHUTDOWN
                else if (userInput.equals("shutdown")) {
                    printWriter.println(userInput);
                    String response = bufferedReader.readLine();
                    System.out.println(response);
                    if (response.equals("200 OK")) {
                        break;
                    }
                }

                // LOGOUT
                else if (userInput.equals("logout")) {
                    printWriter.println(userInput);
                    String response = bufferedReader.readLine();
                    System.out.println(response);
                }

                // LOGIN user pass
                else if (userInput.startsWith("login ")) {
                    printWriter.println(userInput);
                    String response = bufferedReader.readLine();
                    System.out.println(response);
                }

                // MSGSTORE message
                else if (userInput.startsWith("msgstore ")) {
                    printWriter.println(userInput);
                    String response = bufferedReader.readLine();
                    System.out.println(response);
                } else {
                    System.out.println("Invalid command.");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (printWriter != null) {
                    printWriter.close();
                }
                if (bufferedReader != null) {
                    bufferedReader.close();
                }
                if (s != null) {
                    s.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}

import java.io.*;
import java.net.*;
/** Server.java, 
 * this server class is responsible for handling client connection */
public class Server {
    //contants
    private static final int SERVER_PORT = 5206;
    private static final String FILE_NAME = "message.txt";
    public static void main(String[] args) {
        // User credentials 
        String user1 = "login root root01";
        String user2 = "login john john01";
        String user3 = "login david david01";
        String user4 = "login mary mary01";

        Socket cSocket = null;
        PrintWriter printWriter = null;
        BufferedReader bufferedReader = null;
        int currentMessageIndex = 0;
        String[] messages = new String[20];
        int totalMessages = 0;
        boolean isRootUserLoggedIn = false;
        boolean isUserLoggedIn = false;
        boolean shutdown = false;

        // Read initial messages from a file
        try (BufferedReader fileReader = new BufferedReader(new FileReader(FILE_NAME))) {
            String line;
            int j = 0;
            while ((line = fileReader.readLine()) != null && j < 20) {
                messages[j] = line;
                j++;
                totalMessages++;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        try (ServerSocket serverSocket = new ServerSocket(SERVER_PORT)) {
            System.out.println("Welcome to Server.");
            System.out.println("The server is ready, waiting for client connection");

            while (!shutdown) {
                cSocket = serverSocket.accept();
                System.out.println("New Connection Received From " + cSocket.getInetAddress().getHostAddress());

                try {
                    printWriter = new PrintWriter(cSocket.getOutputStream(), true);
                    bufferedReader = new BufferedReader(new InputStreamReader(cSocket.getInputStream()));
                    String inputLine;

                    while ((inputLine = bufferedReader.readLine()) != null && !shutdown) {
                        System.out.println(inputLine.toUpperCase());

                        // Shutdown Command
                        if (inputLine.equals("shutdown")) {
                            if (isRootUserLoggedIn) {
                                shutdown = true;
                                printWriter.println("200 OK");
                                break;
                            } else {
                                printWriter.println("402 User not allowed to execute this command.");
                            }
                        }

                        // Quit Command
                        else if (inputLine.equals("quit")) {
                            printWriter.println("200 OK");
                            break;
                        }

                        // MSGGET Command
                        else if (inputLine.equals("msgget")) {
                            printWriter.println("200 OK");
                            printWriter.println(messages[currentMessageIndex]+"\n");
                            
                            currentMessageIndex++;
                            if(currentMessageIndex==totalMessages){
                                currentMessageIndex=0;
                            }
                        }

                        // Logout Method
                        else if (inputLine.equals("logout")) {
                            if (isRootUserLoggedIn) {
                                printWriter.println("200 OK");
                                isRootUserLoggedIn = false;
                            } else if (isUserLoggedIn) {
                                printWriter.println("200 OK");
                                isUserLoggedIn = false;
                            } else {
                                printWriter.println("No users logged in");
                            }
                        }

                        // Login Method
                        else if (inputLine.equals(user1) || inputLine.equals(user2) ||
                                inputLine.equals(user3) || inputLine.equals(user4)) {
                            printWriter.println("200 OK");
                            if (inputLine.equals(user1)) {
                                isRootUserLoggedIn = true;
                            } else {
                                isUserLoggedIn = true;
                            }
                        } else if (inputLine.equals("login")) {
                            printWriter.println("410 Wrong UserID or Password");
                        }

                        // MSGStore
                        else if (inputLine.startsWith("msgstore")) {
                            String message = inputLine.substring(9);
                            if (isUserLoggedIn || isRootUserLoggedIn) {
                                if (totalMessages < 20) {
                                    messages[totalMessages] = message;
                                    totalMessages++;
                                    try (BufferedWriter fileWriter = new BufferedWriter(new FileWriter(FILE_NAME, true))) {
                                        fileWriter.write(message);
                                        fileWriter.newLine();
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }
                                    printWriter.println("200 OK");
                                } else {
                                    printWriter.println("402 No more space max limit exceeded");
                                }
                            } else {
                                printWriter.println("401 You are not currently logged in, login first");
                            }
                        }

                        // Invalid Command
                        else {
                            printWriter.println("400 Bad Request");
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    if (printWriter != null) {
                        printWriter.close();
                    }
                    if (bufferedReader != null) {
                        bufferedReader.close();
                    }
                    if (cSocket != null) {
                        cSocket.close();
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

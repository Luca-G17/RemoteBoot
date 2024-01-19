#include <Arduino.h>
#include <WiFi.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define WIFI_SSID "VM6127049"
#define WIFI_PWD  "cxdw3ZvQndtr"
#define WIFI_TIMEOUT_MS 20000
#define TCP_PORT  2000


void boot() {
  digitalWrite(16, HIGH);
  delay(1000);
  digitalWrite(16, LOW);
}

int tcp_connection() {
  int socket_fd, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[2000], client_message[2000];

  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));
  
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    Serial.printf("Error while creating socket: %s\n", strerror(errno));
    return -1;
  }

  const int enable = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    Serial.printf("Error setting socket option - SO_REUSEADDR: %s\n", strerror(errno));
  }

  
  // if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
  //   Serial.printf("Error setting socket option - SO_REUSEPORT: %s\n", strerror(errno));
  // }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(TCP_PORT);
  server_addr.sin_addr.s_addr = inet_addr("192.168.0.131");

  if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    Serial.printf("Failed to bind to port: %s\n", strerror(errno));
    close(socket_fd);
    return -1;
  }

  if (listen(socket_fd, 1) < 0) {
    Serial.println("Failed to listen to socket");
    close(socket_fd);
    return -1;
  }

  Serial.println("Listening for connections...");
  client_size = sizeof(client_addr);
  client_sock = accept(socket_fd, (struct sockaddr*)&client_addr, &client_size);
  Serial.println("Recieving Connection");
  if (client_sock < 0) {
    Serial.println("Failed to accept connection");
    close(socket_fd);
    return -1;
  }

  Serial.printf("Client Connected - IP = %s\n", inet_ntoa(client_addr.sin_addr));

  if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
    Serial.println("Failed to recieve message");
    return -1;
  }

  Serial.printf("Message from client: %s\n", client_message);
  char os_str[40];
  char boot_str[40];
  sscanf(client_message, "%s - %s", boot_str, os_str);
  if (strcmp(boot_str, "BOOT") == 0) {
    if (strcmp(os_str, "LINUX") == 0) {
      digitalWrite(21, HIGH);
      boot();
    } 
    else if (strcmp(os_str, "WINDOWS") == 0) {
      digitalWrite(21, LOW);
      boot();
    }
  }

  close(client_sock);
  close(socket_fd);
  return 0;
}

void tcp_manager(void* parameter) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
      continue;
    }
    tcp_connection();
  }
}


void wifi_connection(void* parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PWD);

    unsigned long startAttemptTime = millis();
    while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {}

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connection attempt timed out");
      vTaskDelay(WIFI_TIMEOUT_MS / portTICK_PERIOD_MS);
    } else {
      Serial.printf("WiFi Connected: %s - IP: %s\n", WIFI_SSID, WiFi.localIP().toString());
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  pinMode(21, OUTPUT);
  delay(1000);

  xTaskCreate(
    wifi_connection,
    "WiFi Routine",
    4000,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    tcp_manager,
    "TCP Server Routine",
    8000,
    NULL,
    1,
    NULL
  );
}

void loop() {
  delay(1000);
}

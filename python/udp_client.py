import socket

# Server details
server_address = ("127.0.0.1", 12345)  # Replace with the server's IP and port

# Create a UDP socket
udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    # Send data
    message = "Hello, UDP Server!"
    udp_client.sendto(message.encode(), server_address)
    print(f"Sent: {message}")

    # Receive response
    data, server = udp_client.recvfrom(4096)
    print(f"Received: {data.decode()}")
finally:
    # Close the socket
    udp_client.close()

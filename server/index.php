<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_data";

$conn = new mysqli($servername, $username, $password);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
$sql = "CREATE DATABASE IF NOT EXISTS $dbname";
if ($conn->query($sql) !== TRUE) {
    die("Error creating database: " . $conn->error);
}
$conn->close();

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "CREATE TABLE IF NOT EXISTS text_history (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    receiver VARCHAR(10) NOT NULL,
    text VARCHAR(50) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)";
if ($conn->query($sql) !== TRUE) {
    die("Error creating table: " . $conn->error);
}

$sql = "SELECT id, receiver, text, timestamp FROM text_history ORDER BY timestamp DESC";
$result = $conn->query($sql);
$history = [];
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $history[] = $row;
    }
}
$conn->close();
?>

<!DOCTYPE html>
<html>

<head>
    <title>ESP32 Text Transmitter</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            padding: 20px;
        }

        input {
            padding: 10px;
            margin: 10px;
            width: 80%;
        }

        button {
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            cursor: pointer;
        }

        button:hover {
            background-color: #45a049;
        }

        table {
            width: 80%;
            margin: 20px auto;
            border-collapse: collapse;
        }

        th,
        td {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: left;
        }

        th {
            background-color: #f2f2f2;
        }
    </style>
</head>

<body>
    <h1>ESP32 Text Transmitter</h1>
    <h2>Send to Receiver 1</h2>
    <form action="post-text.php" method="POST">
        <input type="hidden" name="receiver" value="Receiver 1">
        <input type="text" name="text" placeholder="Enter text for Receiver 1">
        <button type="submit">Send to Receiver 1</button>
    </form>
    <h2>Send to Receiver 2</h2>
    <form action="post-text.php" method="POST">
        <input type="hidden" name="receiver" value="Receiver 2">
        <input type="text" name="text" placeholder="Enter text for Receiver 2">
        <button type="submit">Send to Receiver 2</button>
    </form>
    <h2>Text History</h2>
    <table>
        <tr>
            <th>ID</th>
            <th>Receiver</th>
            <th>Text</th>
            <th>Timestamp</th>
        </tr>
        <?php foreach ($history as $row): ?>
            <tr>
                <td>
                    <?php echo htmlspecialchars($row['id']); ?>
                </td>
                <td>
                    <?php echo htmlspecialchars($row['receiver']); ?>
                </td>
                <td>
                    <?php echo htmlspecialchars($row['text']); ?>
                </td>
                <td>
                    <?php echo htmlspecialchars($row['timestamp']); ?>
                </td>
            </tr>
        <?php endforeach; ?>
    </table>
</body>

</html>

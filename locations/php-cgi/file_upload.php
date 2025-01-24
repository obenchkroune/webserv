<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>webserv</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }

      body {
        min-height: 100vh;
        display: flex;
        justify-content: center;
        align-items: center;
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        font-family: -apple-system, "SF Mono", Inter, sans-serif;
        padding: 20px;
      }

      .container {
        background: rgba(255, 255, 255, 0.95);
        padding: 3rem 4rem;
        border-radius: 20px;
        box-shadow: 0 20px 40px rgba(0, 0, 0, 0.2);
        text-align: center;
        max-width: 600px;
        width: 100%;
      }

      h1 {
        color: #1a1a1a;
        font-size: 2.5rem;
        margin-bottom: 1rem;
        letter-spacing: -0.5px;
      }

      .status {
        color: #4a5568;
        font-size: 1.2rem;
        line-height: 1.6;
      }

      .badge {
        display: inline-block;
        background: #48bb78;
        color: white;
        padding: 0.5rem 1rem;
        border-radius: 9999px;
        font-size: 0.875rem;
        margin-top: 1.5rem;
        font-weight: 500;
      }

      .server-info {
        margin-top: 2rem;
        padding-top: 2rem;
        border-top: 1px solid #e2e8f0;
        font-size: 0.875rem;
        color: #718096;
      }

      a {
        display: inline-block;
        color: #764ba2;
        text-decoration: none;
        padding: 0.25rem 0.5rem;
        border-radius: 4px;
        transition: all 0.2s ease;
      }

      a:hover {
        background: rgba(118, 75, 162, 0.1);
        text-decoration: none;
        color: #5a3a7e;
      }

      .server-info p {
        margin-block: 1em;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Webserv/php-cgi</h1>
      <div class="badge">php cgi is up and running</div>
    <div class='status'>File Upload</div>
    <?php
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            if (isset($_POST["file_description"]) && isset($_FILES["file"]))
            {
                echo "<div class='status'>Your file was uploaded correctly.</div>";
                echo "<div class='status'>File description: ".$_POST["file_description"]."</div>";
                echo "<div class='status'>File name: ".$_FILES["file"]['name']."</div>";
                echo "<div class='status'>File full path: ".$_FILES["file"]['full_path']."</div>";
                echo "<div class='status'>File size: ".$_FILES["file"]['size']."</div>";
                echo "<div class='status'>File error code: ".$_FILES["file"]['error']."</div>";
                echo "<div class='status'>File tmp name: ".$_FILES["file"]['tmp_name']."</div>";
                move_uploaded_file($_FILES["file"]['tmp_name'], "./uploaded-files/".$_FILES["file"]['name']);
            }
            else
            {
                echo "<div class='status'>Error something went wrong.</div>";
            }
        }
    ?>
    <div class='status'>Upload your file here:</div>
    <form action="file_upload.php" method="post" enctype="multipart/form-data">
        <label for="file_description">File Description:</label>
        <input type="text" id="file_description" name="file_description"><br><br>
        <label for="file">Select a file:</label>
        <input type="file" id="file" name="file"><br><br>
        <input type="submit" value="Submit">
    </form>
    <div class="server-info">
        <p>Built with C++98 | HTTP/1.1 Compliant</p>
        <a href="/">main</a>
        |
        <a href="/php-cgi/php_info.php">php_info</a>
        |
        <a href="https://github.com/obenchkroune/webserv" target="_blank">
          View on GitHub
        </a>
      </div>
    </div>
  </body>
</html>
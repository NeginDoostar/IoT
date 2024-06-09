<?php

	$target_dir = "uploads/";
	$datum = mktime(date('H')+0, date('i'), date('s'), date('m'), date('d'), date('y'));
	$target_file = $target_dir . date('Y.m.d_H:i:s_', $datum) . basename($_FILES["imageFile"]["name"]);
	$uploadOk = 1;
	$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));


	// Check if image file is a actual image or fake image
	if(isset($_POST["submit"])) 
	{
	  $check = getimagesize($_FILES["imageFile"]["tmp_name"]);
	  if($check !== false)
	  {
		echo "File is an image - " . $check["mime"] . ".";
		$uploadOk = 1;
	  }
	  else 
	  {
		echo "File is not an image.";
		$uploadOk = 0;
	  }
	}


	// Check if file already exists
	if (file_exists($target_file)) 
	{
	  echo "Sorry, file already exists.";
	  $uploadOk = 0;
	}


	// Check file size
	if ($_FILES["imageFile"]["size"] > 500000) 
	{
	  echo "Sorry, your file is too large.";
	  $uploadOk = 0;
	}


	// Allow certain file formats
	if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
	&& $imageFileType != "gif" ) {
	  echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.";
	  $uploadOk = 0;
	}


	// Check if $uploadOk is set to 0 by an error
	if ($uploadOk == 0) 
	{
	  echo "Sorry, your file was not uploaded.";
	  // if everything is ok, try to upload file
	}
	else 
	{
	  //add image_name to db
	  if (move_uploaded_file($_FILES["imageFile"]["tmp_name"], $target_file)) 
	  {
		$servername = "localhost";
		$username = "negindo6_esp32";
		$password = "neginESP23#2$";
		$dbname = "negindo6_esp32";

		// Create connection
		$conn = mysqli_connect($servername, $username, $password, $dbname);
		// Check connection
		if (!$conn) {
		  die("Connection failed: " . mysqli_connect_error());
		}

		$sql = "INSERT INTO photos (name)
		VALUE ('http://negindoostar.ir/uploads/".date('Y.m.d_H:i:s_', $datum) . basename($_FILES["imageFile"]["name"])."')";

		if (mysqli_query($conn, $sql)) {
		  echo "http://negindoostar.ir/uploads/".date('Y.m.d_H:i:s_', $datum) . basename($_FILES["imageFile"]["name"]);
		} else {
		  echo "Error: " . $sql . "<br>" . mysqli_error($conn);
		}

		mysqli_close($conn);
		
	  }
	  else 
	  {
		echo "Sorry, there was an error uploading your file.";
	  }
	}

?>
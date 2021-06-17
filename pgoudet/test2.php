<?php
$post_data = "userid=";
$post_data .= $_POST['userid'];

$curl = curl_init();

curl_setopt($curl, CURLOPT_URL, "http://ww7.aitsafe.com/cf/addmulti.cfm");
curl_setopt($curl, CURLOPT_POST, 1);
curl_setopt($curl, CURLOPT_POSTFIELDS, $post_data);
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
$page = curl_exec($curl);
curl_close($curl);

print($page);
?>
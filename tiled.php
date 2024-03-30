<?php
$file = "gng.tmx";

if (file_exists($file)) {
    $xml = simplexml_load_file($file);
    $json = json_encode($xml);
    $array = json_decode($json, TRUE);

    foreach ($array["layer"] as $key => $value) {
        if ( $value["@attributes"]["name"] != "test" ){

            if ( $value["@attributes"]["name"] == "nuages" || 1 ){

                $nombre_tiles_width = 224;
                if ( isset($value["properties"]["property"][1]["@attributes"]["name"]) && $value["properties"]["property"][1]["@attributes"]["name"] == "width" ) {
                    $nombre_tiles_width = $value["properties"]["property"][1]["@attributes"]["value"];
                }
                $nombre_tiles_height = 15;
                if ( isset($value["properties"]["property"][0]["@attributes"]["name"]) && $value["properties"]["property"][0]["@attributes"]["name"] == "height" ) {
                    $nombre_tiles_height = $value["properties"]["property"][0]["@attributes"]["value"];
                }

                $data = $value["data"];
                echo "const u16 tmx_" . $value["@attributes"]["name"] . "[".$nombre_tiles_height."][".$nombre_tiles_width."] = {\n";
                $ligne_cpt = 0;
                
                $cpt = 0;
                foreach (explode("\n", $data) as $ligne) {

                    if (!empty($ligne)) {

                        $cpt++;
                        if ( $cpt <= $nombre_tiles_height ){

                            $ligne_cpt++;
                            $ligne = rtrim($ligne, ",");

                            $tab_ligne = explode(",", $ligne);
                            $tmp_array = [];
                            for($i=0;$i<$nombre_tiles_width;$i++){
                                $tmp_array[$i] = $tab_ligne[$i];
                            }

                            $display = true;
                            if ($value["@attributes"]["name"] == "herbe" && $ligne_cpt < 14) {
                                //$display = false;
                            }

                            if ($display) {
                                echo "\t{" . implode(",", $tmp_array) . "},\n";
                            }
                        }
                    }
                }
                echo "};\n";
            }
        }
    }
}
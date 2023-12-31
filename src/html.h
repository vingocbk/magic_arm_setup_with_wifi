#ifndef html_H
#define html_H


char index_html_handle_root[]  = 
	"<!DOCTYPE HTML><html><head>\n"
		"<title>ESP Input Form</title>\n"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
		"</head><body>\n"
            "<h1>CONFIG SETUP</h1><br>\n"
			"<form action=\"/reset_distant\">\n"
				"<h3>Reset Distant</h3><br>\n"
				"<input type=\"submit\" value=\"Reset Distant\"><br>\n"
			"</form><br>\n"
			"<form action=\"/low_speed\">\n"
				"<h3>Slow Speed</h3><br>\n"
				"<input type=\"text\" name=\"low_speed\">\n"
				"%d\n"
				"<input type=\"submit\" value=\"Save Config\"><br>\n"
			"</form><br>\n"
			"<form action=\"/time_return\">\n"
				"<h3>Time Return</h3><br>\n"
				"<input type=\"text\" name=\"time_return\">\n"
				"%d\n"
				"<input type=\"submit\" value=\"Save Config\"><br>\n"
			"</form><br>\n"
			"<form action=\"/percent_low_speed_in\">\n"
				"<h3>Percent Distant In</h3><br>\n"
				"<input type=\"text\" name=\"percent_low_speed_in\">\n"
				"%d\n"
				"<input type=\"submit\" value=\"Save Config\"><br>\n"
			"</form><br>\n"
			"<form action=\"/percent_low_speed_out\">\n"
				"<h3>Percent Distant Out</h3><br>\n"
				"<input type=\"text\" name=\"percent_low_speed_out\">\n"
				"%d\n"
				"<input type=\"submit\" value=\"Save Config\"><br>\n"
			"</form><br>\n"
			"<form action=\"/min_stop_speed\">\n"
				"<h3>Min Stop Speed</h3><br>\n"
				"<input type=\"text\" name=\"min_stop_speed\">\n"
				"%d\n"
				"<input type=\"submit\" value=\"Save Config\"><br>\n"
			"</form><br>\n"
	    "</body>\n"
    "</html>";

#endif	//	html_H
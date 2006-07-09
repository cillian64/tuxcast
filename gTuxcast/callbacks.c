#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
// TODO: Fix that stuff!!
// (xml2-config, or pkg-config)

#include <string.h>

// TCP stuff:
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


GtkTextBuffer *checkoutputtext;
GtkTextBuffer *up2dateoutputtext;

int TCPconnect(char *host, int port);

void
on_check_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	// Generate a "check" feed from the input
	
	
	// Variables etc. first:
	GtkWidget *input = lookup_widget(button, "checkentry");
	GtkWidget *output = lookup_widget(button, "checkoutput");

	// Make a doc:
	// Staticced:
/*	doc = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewNode(NULL, (xmlChar *)"tuxcast");
	xmlDocSetRootElement(doc, root);
	xmlNewChild(root, NULL, (xmlChar *)"version",
			(xmlChar *)"0.2");
	xmlNewChild(root, NULL, (xmlChar *)"client",
			(xmlChar *)"gTuxcast");
	curr = xmlNewChild(root, NULL, (xmlChar *)"request",
			(xmlChar *)"");
	xmlNewChild(curr, NULL, (xmlChar *)"type",
			(xmlChar *)"Check");
	xmlNewChild(curr, NULL, (xmlChar *)"feed",
		(xmlChar *)gtk_entry_get_text(GTK_ENTRY(input)));
	
	xmlDocDump(thefile, doc);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	*/

	
	
//	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(checkoutputtext), 
	//		buffer.buffer->content
	//		, -1);
	// !??!??!?

}


void
on_checkall_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_text_buffer_set_text(checkoutputtext, "Sorry, checkall isn't implemented yet", -1);
}


void
on_up2date_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_text_buffer_set_text(up2dateoutputtext, "Sorry, up2date isn't implemented yet", -1);

}


void
on_up2dateall_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_text_buffer_set_text(up2dateoutputtext, "Sorry, up2dateall isn't implemented yet", -1);
}


int TCPconnect(char *host, int port)
{
	// Variables, etc:
	struct in_addr remoteaddr;
	struct sockaddr_in server;
	int sock;

	// ...
}

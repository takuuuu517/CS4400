/*
 * redpin.c - [Starting code for] a web-based manager of people and
 *            places.
 *
 * Based on:
 *  tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *      GET method to serve static and dynamic content.
 *   Tiny Web server
 *   Dave O'Hallaron
 *   Carnegie Mellon University
 */
#include "csapp.h"
#include "dictionary.h"
#include "more_string.h"

static void doit(int fd);
static dictionary_t *read_requesthdrs(rio_t *rp);
static void read_postquery(rio_t *rp, dictionary_t *headers, dictionary_t *d);
static void clienterror(int fd, char *cause, char *errnum,
                        char *shortmsg, char *longmsg);
static void print_stringdictionary(dictionary_t *d);
static void serve_request(int fd, dictionary_t *query);


static dictionary_t *people;
static dictionary_t *places;
static void serve_request_counts(int fd, dictionary_t *query);
static void serve_request_reset(int fd, dictionary_t *query);
static void serve_request_people(int fd, dictionary_t *query, char *uri);
static void serve_request_place(int fd, dictionary_t *query);
static void serve_request_pin(int fd, dictionary_t *query);
static void serve_request_unpin(int fd, dictionary_t *query);
static void serve_request_copy(int fd, dictionary_t *query);

static void *doit_th(void *connfdp);



int main(int argc, char **argv)
{
  people = make_dictionary(COMPARE_CASE_SENS,  (free_proc_t)dictionary_free); // people dictionary (person -> person dictionary)
  places = make_dictionary(COMPARE_CASE_SENS,   (free_proc_t)dictionary_free); // place dictionary  (place -> place dictionary)

  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);

  /* Don't kill the server if there's an error, because
     we want to survive errors due to a client. But we
     do want to report errors. */
  exit_on_error(0);

  /* Also, don't stop on broken connections: */
  Signal(SIGPIPE, SIG_IGN);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (connfd >= 0) {
      Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
                  port, MAXLINE, 0);
      // printf("Accepted connection from (%s, %s)\n", hostname, port);
      // doit(connfd);

      int *connfdp;
      pthread_t th;
      connfdp = malloc(sizeof(int));
      *connfdp = connfd;
      Pthread_create(&th, NULL, doit_th, connfdp);            //
      Pthread_detach(th);
      // Close(connfd);
    }
  }
}

void *doit_th(void *connfdp) // wrapper for doit method
{
  int connfd = *(int *)connfdp;
  free(connfdp); //connfdp is not needed anymore
  doit(connfd);
  Close(connfd);
  return NULL;
}


/*
 * doit - handle one HTTP request/response transaction
 */
void doit(int fd)
{
  char buf[MAXLINE], *method, *uri, *version;
  rio_t rio;
  dictionary_t *headers, *query;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  if (Rio_readlineb(&rio, buf, MAXLINE) <= 0)
    return;
  // printf("%s", buf);

  if (!parse_request_line(buf, &method, &uri, &version)) {
    clienterror(fd, method, "400", "Bad Request",
                "Redpin did not recognize the request");
  } else {
    if (strcasecmp(version, "HTTP/1.0")
        && strcasecmp(version, "HTTP/1.1")) {
      clienterror(fd, version, "501", "Not Implemented",
                  "Redpin does not implement that version");
    } else if (strcasecmp(method, "GET")
               && strcasecmp(method, "POST")) {
      clienterror(fd, method, "501", "Not Implemented",
                  "Redpin does not implement that method");
    } else {
      headers = read_requesthdrs(&rio);

      /* Parse all query arguments into a dictionary */
      query = make_dictionary(COMPARE_CASE_SENS, free);
      parse_uriquery(uri, query);
      if (!strcasecmp(method, "POST"))
        read_postquery(&rio, headers, query);

      /* For debugging, print the dictionary */
      // printf("hello %s\n",uri );
      print_stringdictionary(query);
      // printf("hello %s\n",uri );

      if (starts_with("/counts", uri)){
        // printf("counts\n");
        serve_request_counts(fd, query);

      }else if(starts_with("/reset", uri)){
        // printf("reset\n");
        serve_request_reset(fd, query);

      }else if(starts_with("/people", uri)){
        serve_request_people(fd, query, uri);
        // printf("people\n");

      }else if(starts_with("/place", uri)){
        serve_request_place(fd, query);
        // printf("place\n");

      }else if(starts_with("/pin", uri)){
        serve_request_pin(fd, query);
        // printf("pin\n");

      }else if(starts_with("/unpin", uri)){
        serve_request_unpin(fd, query);
        // printf("unpin\n");

      }else if(starts_with("/copy", uri)){
        serve_request_copy(fd, query);
        // printf("copy\n");
      }else
      {
        clienterror(fd, "", "400", "Not valid URL", "URL is not valid ");
        // printf("This URL is not valid\n" );
      }



      /* You'll want to handle different queries here,
         but the intial implementation always returns
         nothing: */
      // serve_request(fd, query);

      /* Clean up */
      dictionary_free(query);
      dictionary_free(headers);

    }

    /* Clean up status line */
    free(method);
    free(uri);
    free(version);
  }
}

/*
 * read_requesthdrs - read HTTP request headers
 */
dictionary_t *read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];
  dictionary_t *d = make_dictionary(COMPARE_CASE_INSENS, free);

  Rio_readlineb(rp, buf, MAXLINE);
  while(strcmp(buf, "\r\n")) {
    printf("%s", buf);
    parse_header_line(buf, d);
    Rio_readlineb(rp, buf, MAXLINE);
  }

  return d;
}

void read_postquery(rio_t *rp, dictionary_t *headers, dictionary_t *dest)
{
  char *len_str, *type, *buffer;
  int len;

  len_str = dictionary_get(headers, "Content-Length");
  len = (len_str ? atoi(len_str) : 0);

  type = dictionary_get(headers, "Content-Type");

  buffer = malloc(len+1);
  Rio_readnb(rp, buffer, len);
  buffer[len] = 0;

  if (!strcasecmp(type, "application/x-www-form-urlencoded")) {
    parse_query(buffer, dest);
  }

  free(buffer);
}

static char *ok_header(size_t len, const char *content_type) {
  char *len_str, *header;

  header = append_strings("HTTP/1.0 200 OK\r\n",
                          "Server: Redpin Web Server\r\n",
                          "Connection: close\r\n",
                          "Content-length: ", len_str = to_string(len), "\r\n",
                          "Content-type: ", content_type, "\r\n\r\n",
                          NULL);
  free(len_str);

  return header;
}

/*
 * serve_request - example request handler
 */
static void serve_request(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;
  body = strdup("alice\nbob\n");
  len = strlen(body);

  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);

  free(header);

  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum,
		 char *shortmsg, char *longmsg)
{
  size_t len;
  char *header, *body, *len_str;

  body = append_strings("<html><title>Redpin Error</title>",
                        "<body bgcolor=""ffffff"">\r\n",
                        errnum, " ", shortmsg,
                        "<p>", longmsg, ": ", cause,
                        "<hr><em>Redpin Server</em>\r\n",
                        NULL);
  len = strlen(body);

  /* Print the HTTP response */
  header = append_strings("HTTP/1.0 ", errnum, " ", shortmsg, "\r\n",
                          "Content-type: text/html; charset=utf-8\r\n",
                          "Content-length: ", len_str = to_string(len), "\r\n\r\n",
                          NULL);
  free(len_str);

  Rio_writen(fd, header, strlen(header));
  Rio_writen(fd, body, len);

  free(header);
  free(body);
}

static void print_stringdictionary(dictionary_t *d)
{
  int i;
  const char **keys;

  keys = dictionary_keys(d);

  for (i = 0; keys[i] != NULL; i++) {
    printf("%s=%s\n",
           keys[i],
           (const char *)dictionary_get(d, keys[i]));
  }
  printf("\n");

  free(keys);
}


static void serve_request_counts(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;

  int poeple_num = dictionary_count(people);
  int places_num = dictionary_count(places);

  char* pecount = to_string(poeple_num);
  char* plcount = to_string(places_num);



  char* result = append_strings(pecount,"\n",plcount,"\n",NULL);

  free(pecount);
  free(plcount);

  // char* result = append_strings(to_string(poeple_num),"\n",to_string(places_num),"\n",NULL);
  body = strdup(result);
  len = strlen(body);

  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);

  free(header);


  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
  free(result);
}

static void serve_request_reset(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;

  // dictionary_free(people);
  // dictionary_free(places);

  int poeple_num = dictionary_count(people);
  int places_num = dictionary_count(places);
  char* result;

  if(!(places_num ==0  && poeple_num ==0))
  {
    // printf("poeple_numpoeple_numpoeple_numpoeple_num %d\n",poeple_num );

    const char** people_key = dictionary_keys(people);
    const char** place_key = dictionary_keys(places);
    // printf("%s\n",place_key[0] );


    int i, j ;
    for(i = 0; i < poeple_num; i++)
    {
      // printf("%s\n","ashfkashkfhasfkds" );
      // printf("%s\n",people_key[i] );
      // dictionary_free(dictionary_get(people,people_key[i]));
      dictionary_remove(people, people_key[i]);

    }
    int places_num = dictionary_count(places);
    printf("places_numplaces_numplaces_numplaces_num %d\n",places_num );

    for(j = 0; j < places_num; j++)
    {
      // printf("%s\n","ashfkashkfhasfkds" );
      // printf("%s\n",place_key[j] );
      // dictionary_free(dictionary_get(places,place_key[i]));
      dictionary_remove(places, place_key[j]);
    }


    free(people_key);
    free(place_key);
  }
  char* pecount = to_string(dictionary_count(people));
  char* plcount = to_string(dictionary_count(places));


  result = append_strings(pecount,"\n",plcount,"\n",NULL);
  free(pecount);
  free(plcount);

  body = strdup(result);

  len = strlen(body);

  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);

  free(header);


  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
  free(result);
}

static void serve_request_people(int fd, dictionary_t *query, char* uri)
{
  int i;

  size_t len;
  char *body, *header;
  int flag = 0;


  // implementation
  char *place_s = dictionary_get(query, "place"); // NAME OF THE PLACE IF ANY


  // char * result ="";
  if(place_s == NULL) // WHEN PLACE IS NOT SPECIFIED -> return all the people
  {



    // printf("testtesttesttesttesttesttesttest\n" );
    // printf("%s\n",keys[0]);
    // printf("%s\n",keys[1]);
    // printf("testtesttesttesttesttesttesttest\n" );
    printf("place not specitied\n" );
    printf("dictionary_count(people)   %d\n", dictionary_count(people) );


    if(dictionary_count(people) == 0 ) // there is not people
      flag = 1;
      // result  = "\n";
    else
    {
      const char** keys = dictionary_keys(people);
      char* result = join_strings(keys, '\n');

      // for(i = 0 ; i < dictionary_count(people) ; i++ )
      // {
      //   result = append_strings(result, keys[i],"\n", NULL);
      // }
      body = strdup(result);
      len = strlen(body);
      // implementation
      /* Send response headers to client */
      header = ok_header(len, "text/plain; charset=utf-8");
      Rio_writen(fd, header, strlen(header));
      printf("Response headers:\n");
      printf("%s", header);
      free(header);
      /* Send response body to client */
      Rio_writen(fd, body, len);
      free(body);
      free(result);
      free(keys);
    }
    // printf("resultresultresultresult %s\n", result);
  }
  else // WHEN PLACE IS SPECIFIED (place\nplace)
  {
    if(dictionary_has_key(places, place_s))
    {
      dictionary_t *place_people = dictionary_get(places, place_s);

      if(dictionary_count(place_people) == 0 ) // there is not people
        flag = 1;
        // result  = "\n";
      else
      {
        const char** keys = dictionary_keys(place_people);
        char* result = join_strings(keys, '\n');

        // for(i = 0 ; i < dictionary_count(place_people) ; i++ )
        //   result = append_strings(result, keys[i],"\n", NULL);
        body = strdup(result);
        len = strlen(body);
        // implementation
        /* Send response headers to client */
        header = ok_header(len, "text/plain; charset=utf-8");
        Rio_writen(fd, header, strlen(header));
        printf("Response headers:\n");
        printf("%s", header);
        free(header);
        /* Send response body to client */
        Rio_writen(fd, body, len);
        free(body);
        free(result);
        free(keys);
        return ;

      }
    }
    else
      flag = 1;
    // result  = "\n";
  }
  char* result = "\n";
  body = strdup(result);
  len = strlen(body);
  // implementation
  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);
  free(header);
  /* Send response body to client */
  Rio_writen(fd, body, len);
  free(body);
  // free(result);
}

static void serve_request_place(int fd, dictionary_t *query)
{
  int i;

  size_t len;
  char *body, *header;
  int flag = 0;
  // char* result="";


  // implementation
  char *people_s = dictionary_get(query, "person"); // NAME OF THE PLACE IF ANY
  // char * result="";
  if(people_s== NULL) // WHEN PLACE IS NOT SPECIFIED
  {
    const char** keys = dictionary_keys(places);
    printf("%s\n","aksdhf;kashdf;kas" );

    if(dictionary_count(places) == 0 ) // there is not people
      flag = 1;
      // result  = "\n";
    else
    {
      printf("aksflajslflas:   %d\n",dictionary_count(places) );
      printf("keys[0]: %s\n", keys[0]);
      printf("keys[1]: %s\n", keys[1]);
      char* result = join_strings(keys, '\n');
      printf("resultresultresultresultresultresultresult  %s\n",result  );

      // for(i = 0 ; i < dictionary_count(places) ; i++ )
      //   result = append_strings(result, keys[i],"\n", NULL);

      body = strdup(result);
      len = strlen(body);
      // implementation
      /* Send response headers to client */
      header = ok_header(len, "text/plain; charset=utf-8");
      Rio_writen(fd, header, strlen(header));
      printf("Response headers:\n");
      printf("%s", header);
      free(header);
      /* Send response body to client */
      Rio_writen(fd, body, len);

      free(body);
      free(result);
      free(keys);
      return;
    }
    free(keys);
    // return;
  }
  else // WHEN PLACE IS SPECIFIED (people)
  {
    printf("PLACE IS SPECIFIED PLACE IS SPECIFIED PLACE IS SPECIFIED PLACE IS SPECIFIED \n" );
    if(dictionary_has_key(people, people_s))
    {

      printf("yestestes\n" );

      dictionary_t *people_place = dictionary_get(people, people_s);
      const char** keys = dictionary_keys(people_place);


      if(dictionary_count(people_place) == 0 ) // there is not people
        flag = 1;
        // result  = "\n";
      else
      {
        char* result = join_strings(keys, '\n');
        printf("resultresultresultresultresultresultresult  %s\n",result  );


        body = strdup(result);
        len = strlen(body);
        // implementation
        /* Send response headers to client */
        header = ok_header(len, "text/plain; charset=utf-8");
        Rio_writen(fd, header, strlen(header));
        printf("Response headers:\n");
        printf("%s", header);
        free(header);
        /* Send response body to client */
        Rio_writen(fd, body, len);

        free(body);
        free(result);

        // for(i = 0 ; i < dictionary_count(people_place) ; i++ )
        // {
        //   result = append_strings(result, keys[i],"\n", NULL);
        // }
        free(keys);
        return;


      }

      free(keys);
      // return;

    }
    else
      flag = 1;
      // result = "\n";
  }
  char* result  = "\n";
  // if(flag == 1)
  //   result = "\n";

  body = strdup(result);
  len = strlen(body);
  // implementation
  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);
  free(header);
  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
  // free(result);
  return ;

}

static void serve_request_pin(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;

  // implementation
  char *people_s = dictionary_get(query, "people"); // NAME OF THE PLACE IF ANY
  char *place_s = dictionary_get(query, "places"); // NAME OF THE PLACE IF ANY
  printf("people_speople_speople_s %s\n", people_s);
  printf("place_splace_splace_s %s\n", place_s);

  if((place_s == NULL && people_s ==NULL) || place_s == NULL||people_s == NULL )
  {
    clienterror(fd, "", "400", "not valid", "not valid");
    return;
  }

  // create arrays that store people to be pined and the places that people have visited.
  char** people_arr = split_string(people_s, '\n');
  char** places_arr = split_string(place_s, '\n');

  char** people_p; // each person
  char** place_p;
  int i =0 ;
  int j =0 ;

  for(people_p = people_arr; *people_p; people_p++)  // iterate each person and pin them.
  {
    dictionary_t *people_place , *place_people;

    if(!dictionary_has_key(people, *people_p)) // the person already exist in the dictionary
    {
      // people_place = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
      dictionary_set(people, *people_p, make_dictionary(COMPARE_CASE_SENS,  free));
      // dictionary_free(people_place);
    }
    // else
    // {
      people_place = dictionary_get(people, *people_p);
    // }
    // dictionary_t *people_place = dictionary_get(people, *people_p);

    for(place_p = places_arr; *place_p; place_p++)  // iterate each place and pin them.
    {
      if(!dictionary_has_key(people_place, *place_p))
      {
        dictionary_set(people_place, *place_p, NULL);

        // need to make sure that the place exist. if not add to the dictionary
        if(!dictionary_has_key(places,*place_p))
        {
          // add the person.
          // place_people = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
          dictionary_set(places,*place_p ,   place_people = make_dictionary(COMPARE_CASE_SENS,  free));
          // dictionary_free(place_people);
          // free(new_place);
          // dictionary_free(new_place);
        }
        // else
          place_people = dictionary_get(places, *place_p);

        dictionary_set(place_people,*people_p , NULL);

      }


    }

    free(people_arr[i]);
    i++;

  }

  for(place_p = places_arr; *place_p; place_p++)
  {
    free(places_arr[j]);
    j++;
  } // iterate each place and pin them.


  free(people_arr);
  free(places_arr);

  // to get the number of the people and the places //
  int poeple_num = dictionary_count(people);
  int places_num = dictionary_count(places);

  char* pecount = to_string(poeple_num);
  char* plcount = to_string(places_num);



  char* result = append_strings(pecount,"\n",plcount,"\n",NULL);

  free(pecount);
  free(plcount);



  // char* result = append_strings(to_string(poeple_num),"\n",to_string(places_num),"\n",NULL);
  body = strdup(result);
  len = strlen(body);
  // to get the number of the people and the places //

  // implementation


  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);

  free(header);


  /* Send response body to client */
  Rio_writen(fd, body, len);


  free(body);
  free(result);



}

static void serve_request_unpin(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;

  // implementation
  char *people_s = dictionary_get(query, "people"); // NAME OF THE PLACE IF ANY
  char *place_s = dictionary_get(query, "places"); // NAME OF THE PLACE IF ANY

  printf("people_s:  %s\n",people_s );
  printf("place_s:  %s\n", place_s);

  if((place_s == NULL && people_s ==NULL) || place_s == NULL||people_s == NULL )
  {
    clienterror(fd, "", "400", "not valid", "not valid");
    return;
  }

  // create arrays that store people to be unpined and the places.
  char** people_arr = split_string(people_s, '\n');
  char** places_arr = split_string(place_s, '\n');

  char** people_p; // each person
  char** place_p;

  int i = 0;
  int j = 0;
  for(people_p = people_arr; *people_p; people_p++)  // iterate each person and pin them.
  {

    dictionary_t *people_place;
    if(!dictionary_has_key(people, *people_p)) // if the person does not exist in the dictionary -> skip the person
      continue;
    else
      people_place = dictionary_get(people, *people_p);

    for(place_p = places_arr; *place_p; place_p++)  // iterate each place and pin them.
    {
      if(!dictionary_has_key(people_place, *place_p)) // if the person has not visited the place -> nothinkg to do
      {
        continue;
      }
      else // if the person has visited the place -> remove the place
      {
        printf("remove please remove please remove please remove please \n");
        printf("%s\n", *place_p);
        printf("%s\n", *people_p);

        dictionary_remove(people_place, *place_p);

        printf("%d\n", dictionary_count(people_place));

        dictionary_t *place_people = dictionary_get(places, *place_p);
        dictionary_remove(place_people, *people_p);

        if(dictionary_count(people_place)== 0) // if the person lost every place
        {
          // dictionary_free(people_place);

          dictionary_remove(people, *people_p);
          // dictionary_free(place_people);
          // free(place_people);

          // dictiona
          // dictionary_free(dictionary_get(people, *people_p));
        }
        if(dictionary_count(place_people)== 0)
        {
          // dictionary_free(place_people);
          dictionary_remove(places, *place_p);
          // dictionary_free(place_people);
          // free(place_people);

        }
      }
    }

  }

  for(people_p = people_arr; *people_p; people_p++)    // iterate each place and pin them.
  {
    free(people_arr[i]);
    i++;
  }

  for(place_p = places_arr; *place_p; place_p++)  // iterate each place and pin them.
  {
    free(places_arr[j]);
    j++;
  }

  // free(people_s); // いらない
  // free(place_s); // いらない
  free(people_arr);
  free(places_arr);


  // to get the number of the people and the places //
  int poeple_num = dictionary_count(people);
  int places_num = dictionary_count(places);

  char* pecount = to_string(poeple_num);
  char* plcount = to_string(places_num);



  char* result = append_strings(pecount,"\n",plcount,"\n",NULL);

  free(pecount);
  free(plcount);
  body = strdup(result);
  len = strlen(body);
  // to get the number of the people and the places //

  // implementation

  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);
  free(header);

  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
  free(result);


}


static void serve_request_copy(int fd, dictionary_t *query)
{
  size_t len;
  char *body, *header;

  // implementation
  char* host = dictionary_get(query,"host");
  char *port = dictionary_get(query,"port");

  printf("hosthost  %s\n",host);
  printf("portport  %s\n",port);

  char *person = dictionary_get(query,"person");
  char *place = dictionary_get(query,"place");
  char *as = dictionary_get(query,"as");

  if((person == NULL && place ==NULL) || as == NULL||port == NULL || host ==NULL || (person != NULL && place !=NULL))
  {
    clienterror(fd, "", "400", "not valid", "not valid");
    return;
  }


  printf("as as as as as %s\n",as  );
  printf("place place place place place  %s\n",place   );
  printf("person person person person person person %s\n", person);

  char *request;
  // there are two cases: person or place
  if(place == NULL) // if person is specified
  {//GET /copy?host=localhost&port=8090&person=carol&as=alice HTTP/1.1
    request = append_strings("GET /places?person=",person," HTTP/1.1\r\n\r\n",NULL);

  }
  else if(person == NULL)
  {
    request = append_strings("GET /people?place=",place ," HTTP/1.1\r\n\r\n",NULL);

  }
  else
    return ;

  int cfd = Open_clientfd(host, port);

  printf("cfdcfdcfdcfdcfdcfd　　　%d\n",cfd  );

  printf("requestrequestrequestrequestrequest %s\n",request );
  Rio_writen(cfd, request, strlen(request));
  free(request);

  printf("%s\n","write" );

  char buf[MAXLINE], *version, *status, * desc_p;

  rio_t r;
  dictionary_t *headers;

  Rio_readinitb(&r, cfd);

  if (Rio_readlineb(&r, buf, MAXLINE) <= 0)
  {
    printf("%s\n","ERROR" );
    return ;
  }
  printf("bufffff %s\n",buf);


  if(!(parse_status_line(buf, &version, &status, &desc_p)) || strcmp(status, "200")!=0)
  {
    printf("%s\n","ERROR2" );
    return;
  }

  free(status);
  free(desc_p);
  free(version);
  printf("statusstatusstatusstatusstatusstatusstatusstatus %s\n", status);
  headers = read_requesthdrs(&r);

  printf("%s\n","takutakutaku1takutakutaku1takutakutaku1takutakutaku1" );
  print_stringdictionary(headers);
  printf("%s\n","takutakutaku2takutakutaku2takutakutaku2takutakutaku2" );

  char* len_str = dictionary_get(headers, "Content-Length");
  int length = (len_str ? atoi(len_str) : 0);

  char* buffer = malloc(length+1);
  Rio_readnb(&r, buffer, length);
  buffer[length] = 0; // buffer stores the place (place\nplace]n.....)

  printf("bufferbufferbufferbufferbufferbuffer  %s\n",buffer );



  if(strcmp(buffer, "\n") == 0 )// if empty
  {
    printf("%s\n","emptyemptyemptyemptyemptyemptyemptyemptyemptyempty" );
  }
  else if(place == NULL)// there are places
  {
    char** copy_arr;
    copy_arr  = split_string(buffer, '\n');
    int i = 0 ;
    char** copy_p;

    if(!dictionary_has_key(people, as)) // the person already exist in the dictionary
    {
      dictionary_t *new_person = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
      dictionary_set(people, as, new_person);
      // dictionary_free(new_person);
    }
    dictionary_t *people_place = dictionary_get(people, as);

    for(copy_p = copy_arr; *copy_p; copy_p++)  // iterate each copy item
    {
      if(!dictionary_has_key(people_place, *copy_p)) // if the item is not in the dictionary
      {
        dictionary_set(people_place, *copy_p, NULL);

        // need to make sure that the place exist. if not add to the dictionary
        if(!dictionary_has_key(places,*copy_p))
        {
          // add the person.
          dictionary_t *new_place = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
          dictionary_set(places,*copy_p , new_place);
          // dictionary_free(new_place);
        }
        dictionary_t *place_people = dictionary_get(places, *copy_p);
        dictionary_set(place_people,as , NULL);
      }
      free(copy_arr[i]);
      i++;
    }
    free(copy_arr);
    // to get the number of the people and the places //
    // int poeple_num = dictionary_count(people);
    // int places_num = dictionary_count(places);
    // char* result = append_strings(to_string(poeple_num),"\n",to_string(places_num),"\n");
    // body = strdup(result);
    // len = strlen(body);
    // to get the number of the people and the places //

    //copy == pin
  }
  else if(person == NULL)// there are places
  {
    printf("bufferbufferbufferbufferbuffer     %s\n", buffer);
    char** copy_arr;
    copy_arr  = split_string(buffer, '\n');

    char** copy_p;
    int i =0;

    if(!dictionary_has_key(places, as)) // the person already exist in the dictionary
    {
      dictionary_t *new_place = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
      dictionary_set(places, as, new_place);
      // dictionary_free(new_place);
    }
    dictionary_t *place_people = dictionary_get(places, as);

    for(copy_p = copy_arr; *copy_p; copy_p++)  // iterate each copy item
    {
      if(!dictionary_has_key(place_people, *copy_p)) // if the item is not in the dictionary
      {
        dictionary_set(place_people, *copy_p, NULL);

        // need to make sure that the place exist. if not add to the dictionary
        if(!dictionary_has_key(people,*copy_p))
        {
          // add the person.
          dictionary_t *new_people = make_dictionary(COMPARE_CASE_SENS,  free); // people dictionary (person -> person dictionary)
          dictionary_set(people,*copy_p , new_people);
        }
        dictionary_t *people_place = dictionary_get(people, *copy_p);
        dictionary_set(people_place,as , NULL);
      }

      // free(copy_p);

    }

    for(copy_p = copy_arr; *copy_p; copy_p++)  // iterate each copy item
    {
      free(copy_arr[i]);
      i++;
    }
    free(copy_arr);
    // to get the number of the people and the places //
    // int poeple_num = dictionary_count(people);
    // int places_num = dictionary_count(places);
    // char* result = append_strings(to_string(poeple_num),"\n",to_string(places_num),"\n");
    // body = strdup(result);
    // len = strlen(body);
    // to get the number of the people and the places //

    //copy == pin
  }

  // implementation

  dictionary_free(headers);
  free(buffer);

  int poeple_num = dictionary_count(people);
  int places_num = dictionary_count(places);

  char* pecount = to_string(poeple_num);
  char* plcount = to_string(places_num);

  char* result = append_strings(pecount,"\n",plcount,"\n",NULL);

  free(pecount);
  free(plcount);
  // char* result = append_strings(to_string(0),"\n",to_string(0),"\n");
  body = strdup(result);
  len = strlen(body);

  /* Send response headers to client */
  header = ok_header(len, "text/plain; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  printf("Response headers:\n");
  printf("%s", header);
  free(header);


  /* Send response body to client */
  Rio_writen(fd, body, len);

  free(body);
  free(result);
}

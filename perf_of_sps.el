(defun wraprecv(socket)
  (progn
    (print (recv socket))
    (close socket)
    nil))

(defun genematerial(cuid)
  (jtosx  
   (jaddobject 
    (jaddobject 
     (jaddobject (jcreate) 
                 'cuid
                 (jcreatestring  cuid))
     'header
     (jaddobject (jcreate) 
                 'servicekey
                 (jcreatestring  'rd_test)))
    'need_cityid
    (jcreatetrue))))

(defun  post (socket ip data)
  (progn
    (send socket (concat (lineconcat (list (quote POST /UserService/get_user_regular_stay_point/ HTTP/1.1) 
                                           (concat (quote Host: ) ip)
                                           (quote U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (concat (quote Content-Length: ) (itoa (strlen data)))
                                           (quote User-Agent: http_post) 
                                           (quote Content-Type: application/json) 
                                           (quote Accept: */*) 
                                           (quote Accept-Language: utf8) 
                                           (quote Accept-Charset: iso-8859-1,*,utf-8) 
                                           (quote Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (quote Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    (send socket data)
    socket))

(defun  terminal (lst)
  (if (or (big 16 (strlen (car lst)))
          (big (strlen (car lst)) 50))
      nil
    (wraprecv (post (connect (quote  10.94.32.33:8560))
                    (quote   10.94.32.33)
                    (genematerial (car lst))))))

(defun  reactor(file)
  (if (feof file)
      (fclose file)
    (progn
      (pcreate
       1
       'terminal 
       (tabsplit (strip (fgets file))))
      (reactor file))))

(defun detect_hdfs(newname)
  (progn
    (pjoin (pcreate 500 'reactor (fopen newname 'r)))))

(detect_hdfs (print (strip (stdin))))

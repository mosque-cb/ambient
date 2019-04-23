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
    (wraprecv (post (connect (quote 10.94.32.33:8560))
                    (quote   10.94.32.33)
                    (genematerial (car lst))))))

(defun  reactor(file)
  (if (feof file)
      (progn
        (fclose file)
        (print 'next))
    (progn
      (pcreate
       1
       'terminal 
       (tabsplit (strip (fgets file))))
      (reactor file))))

(defun detect_hdfs(name newname)
  (progn
    (system (print (concat (quote ~/hadoop_yq_guiji/bin/../bin/hadoop fs -get  /app/lbs/lbs-guiji/recommend/rds_taotao/user.status.res.offline/20160905/) name 'SPACE (quote .))))
    (system (print (concat (quote  mv ) name 'SPACE newname '.gz)))
    (system (print (concat (quote  gunzip ) newname '.gz)))
    (pjoin (pcreate 500 'reactor (fopen newname 'r)))
    (system (print (concat (quote  rm ) newname)))))

(defun wrap_no(no)
  (if (eq (strlen no) 1) 
      (concat (quote 000) no)
    (if (eq (strlen no) 2) 
        (concat (quote 00) no)
      (if (eq (strlen no) 3) 
          (concat (quote 0) no)
        no))))

(defun dispatch_eval(no)
  (if (big no 1800)
      (print 'over)
    (progn
      (detect_hdfs (concat (quote part-0) 
                           (wrap_no (itoa no))
                           (quote -R.gz))
                   (randomname))
      (dispatch_eval (add no 1)))))

(dispatch_eval (atoi (strip (stdin))))

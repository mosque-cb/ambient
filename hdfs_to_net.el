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
    (send socket (concat (lineconcat (list (storage POST /UserService/get_user_regular_stay_point/ HTTP/1.1) 
                                           (concat (storage Host: ) ip)
                                           (storage U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (concat (storage Content-Length: ) (itoa (strlen data)))
                                           (storage User-Agent: http_post) 
                                           (storage Content-Type: application/json) 
                                           (storage Accept: */*) 
                                           (storage Accept-Language: utf8) 
                                           (storage Accept-Charset: iso-8859-1,*,utf-8) 
                                           (storage Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (storage Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    (send socket data)
    socket))

(defun  terminal (lst)
  (if (or (big 16 (strlen (car lst)))
          (big (strlen (car lst)) 50))
      nil
    (wraprecv (post (connect (storage 10.94.32.33:8560))
                    (storage   10.94.32.33)
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
    (system (print (concat (storage ~/hadoop_yq_guiji/bin/../bin/hadoop fs -get  /app/lbs/lbs-guiji/recommend/rds_taotao/user.status.res.offline/20160905/) name 'SPACE (storage .))))
    (system (print (concat (storage  mv ) name 'SPACE newname '.gz)))
    (system (print (concat (storage  gunzip ) newname '.gz)))
    (pjoin (pcreate 500 'reactor (fopen newname 'r)))
    (system (print (concat (storage  rm ) newname)))))

(defun wrap_no(no)
  (if (eq (strlen no) 1) 
      (concat (storage 000) no)
    (if (eq (strlen no) 2) 
        (concat (storage 00) no)
      (if (eq (strlen no) 3) 
          (concat (storage 0) no)
        no))))

(defun dispatch_eval(no)
  (if (big no 1800)
      (print 'over)
    (progn
      (detect_hdfs (concat (storage part-0) 
                           (wrap_no (itoa no))
                           (storage -R.gz))
                   (randomname))
      (dispatch_eval (add no 1)))))

(dispatch_eval (atoi (strip (stdin))))

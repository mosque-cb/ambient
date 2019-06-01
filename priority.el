(defun mystrdup (material begin end)
  (if (or (eq material nil)
          (eq begin nil)
          (eq end nil))
      nil
    (strdup material 
            begin
            end)))

(defun extract_id (material)
  (if (eq material nil)
      nil
    (mystrdup
     material
     (add 1 (find material (quote >job_)))
     (find material (quote </a></td>)))))

(defun extract_pr(material)
  (if (eq material nil)
      nil
    (mystrdup
     material
     0
     (find material (quote </td>)))))

(defun worker(id pr)
  (if (not (eq pr 'VERY_HIGH))
      (system (concat (quote ~/hadoop_yq_guiji/bin/hadoop  job -set-priority )
                      id
                      'SPACE
                      'VERY_HIGH))
    (print (concat 'pr 'SPACE 'OK))))

(defun decision(jobid priority a b jobname later)
  (progn
    (if (eq (find jobname (quote rosslyn))
            nil)
        nil
      (progn
        (print (concat 'business 'SPACE jobname))
        (worker
         (extract_id (strip jobid))
         (extract_pr (strip priority)))))
    (slot (tail later 14))))

(defun slot (material)
  (if (eq material nil)
      nil
    (decision
     (car material)
     (car (cdr material))
     (car (cdr (cdr material)))
     (car (cdr (cdr (cdr material))))
     (car (cdr (cdr (cdr (cdr material)))))
     (cdr (cdr (cdr (cdr (cdr material))))))))

(defun extract_priority (material)
  (if (eq material nil)
      nil
    (slot (cdr material))))

(defun process(material)
  (if (eq material nil)
      nil
    (extract_priority
     (split material
            (quote <td>)))))

(defun wrapprocess(socket)
  (progn
    (process
     (recv socket 1000000))
    (close socket)))

(defun  get (socket file)
  (progn
    (send socket (concat (lineconcat (list (concat (quote GET ) file 'SPACE (quote HTTP/1.0)) 
                                           (quote Host: api.yeelink.net) 
                                           (quote U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (quote Content-Length: 0)
                                           (quote User-Agent: http_get) 
                                           (quote Content-Type: application/json) 
                                           (quote Accept: */*) 
                                           (quote Accept-Language: utf8) 
                                           (quote Accept-Charset: iso-8859-1,*,utf-8) 
                                           (quote Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (quote Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    socket))

(defun loop()
  (progn
    (print (timetostring (unixtime)))
    (wrapprocess (get (connect (quote  yq01-heng-job.dmop.company.com:8030))
                      (quote /jobqueue.jsp?queueName=lbs-dingwei)))
    (sleep 100)
    (loop)))

(loop)


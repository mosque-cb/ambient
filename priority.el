(defun mystrdup (material begin end)
  (if (or (eq material nil)
          (eq begin nil)
          (eq end nil))
      nil
    (strdup material 
            begin
            end)))

(defun exact_id (material)
  (if (eq material nil)
      nil
    (mystrdup
     material
     (add 1 (find material (storage >job_)))
     (find material (storage </a></td>)))))

(defun exact_pr(material)
  (if (eq material nil)
      nil
    (mystrdup
     material
     0
     (find material (storage </td>)))))

(defun worker(id pr)
  (if (not (eq pr 'VERY_HIGH))
      (system (concat (storage ~/hadoop_yq_guiji/bin/hadoop  job -set-priority )
                      id
                      'SPACE
                      'VERY_HIGH))
    (print (concat 'pr 'SPACE 'OK))))

(defun decision(jobid priority a b jobname later)
  (progn
    (if (eq (find jobname (storage rosslyn))
            nil)
        nil
      (progn
        (print (concat 'business 'SPACE jobname))
        (worker
         (exact_id (strip jobid))
         (exact_pr (strip priority)))))
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

(defun exact_priority (material)
  (if (eq material nil)
      nil
    (slot (cdr material))))

(defun process(material)
  (if (eq material nil)
      nil
    (exact_priority
     (split material
            (storage <td>)))))

(defun wrapprocess(socket)
  (progn
    (process
     (recv socket 1000000))
    (close socket)))

(defun  get (socket file)
  (progn
    (send socket (concat (lineconcat (list (concat (storage GET ) file 'SPACE (storage HTTP/1.0)) 
                                           (storage Host: api.yeelink.net) 
                                           (storage U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (storage Content-Length: 0)
                                           (storage User-Agent: http_get) 
                                           (storage Content-Type: application/json) 
                                           (storage Accept: */*) 
                                           (storage Accept-Language: utf8) 
                                           (storage Accept-Charset: iso-8859-1,*,utf-8) 
                                           (storage Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (storage Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    socket))

(defun loop()
  (progn
    (print (timetostring (unixtime)))
    (wrapprocess (get (connect (storage  yq01-heng-job.dmop.company.com:8030))
                      (storage /jobqueue.jsp?queueName=lbs-dingwei)))
    (sleep 100)
    (loop)))

(loop)


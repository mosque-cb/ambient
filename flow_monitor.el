(setq orcp_bj_nj (list
                  (storage 10.195.91.53:2100)))

(setq nj_old (list
              (storage 10.195.253.15:2100)))

(setq nj_new (list
              (storage 10.195.253.39:2100)))

(defun mystrdup (material begin end)
  (if (or (eq material nil)
          (eq begin nil)
          (eq end nil))
      nil
    (strdup material 
            begin
            end)))

(defun exact_qps_helper (material label)
  (if (eq material nil)
      nil
    (mystrdup
     material
     0
     (find material (storage </span></p>)))))

(defun exact_qps (material label)
  (if (eq material nil)
      nil
    (exact_qps_helper
     (mystrdup material
               (add (strlen label) (find material label) 2)
               (strlen material))
     (storage </span></p>))))

(defun prepare_exact_qps (material)
  (if (eq material nil)
      nil
    (exact_qps
     (mystrdup material (find material (storage qps: )) (strlen material))
     (storage user_service_get_place_semantic_info_qps))))

(defun process(material)
  (if (eq material nil)
      nil
    (prepare_exact_qps
     (mystrdup material
               (find material (storage <h4>GetPlaceSemanticInfo ))
               (strlen material)))))

(defun phone(inf)
  (system (concat (storage gsmsend  -s emp01.company.com:15002  -s emp02.company.com:15002  13641166638@)
                  inf)))

(defun log(ip qps type)
  (print 
   (concat 
    (storage ip is:)
    ip
    'SPACE 
    (storage qps is:)
    qps)))

(defun decision(ip qps type)
  (if (eq type 'orcp_bj_nj)
      (if (big qps 1700)
          (phone (concat (storage big_than_1700_) ip (storage _qps_) qps (storage _type_) type))
        (if (big 100 qps)
            (phone (concat (storage small_than_100_) ip (storage _qps_) qps (storage _type_) type))
          (log ip qps type)))
    (if (eq type 'nj_old)
        (if (big qps 4500)
            (phone (concat (storage big_than_4500_) ip (storage _qps_) qps (storage _type_) type))
          (log ip qps type))
      (if (big qps 4000)
          (phone (concat (storage big_than_4000_) ip (storage _qps_) qps (storage _type_) type))
        (log ip qps type)))))

(defun  fake (material socket)
  (progn
    (close socket)
    material))

(defun  get (socket ip)
  (if (eq socket nil)
      (progn
        (print (concat  ip 'SPACE 'escape))
        0)
    (progn
      (send socket (concat (lineconcat (list (storage GET /status/ HTTP/1.0) 
                                             (storage Host: 127.0.0.1) 
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
      (fake
       (atoi (process 
              (recv socket 1000000)))
       socket))))

(defun trigger(machines type)
  (if (eq machines nil)
      nil
    (progn    
      (decision
       (car machines)
       (get (connect (car machines))
            (car machines))
       type)
      (trigger (cdr machines) type))))

(defun loop()
  (progn
    (print (timestring))
    (print 'orcp_bj_nj)
    (trigger orcp_bj_nj 'orcp_bj_nj)
    (print 'nj_old)
    (trigger nj_old 'nj_old)
    (print 'nj_new)
    (trigger nj_new 'nj_new)
    (sleep 600)
    (loop)))

(loop)


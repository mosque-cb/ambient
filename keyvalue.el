(defun key(db json)
  (if  (big  (nosqlrowid db)   111795)
      json
    (progn
      (jaddarray json (nosqlrow db 1 2))
      (nosqlnext  db)
      (key  db json))))

(defun  test()
  (print (jtosx 
          (jaddobject (jaddobject (jcreate) 
                                  'head 
                                  (jcreatestring 'timecost ))
                      'skulist 
                      (key (nosqlseekgt (nosqlinit 'small.db) 
                                        111720) 
                           (jcreatearray))))))

(test)

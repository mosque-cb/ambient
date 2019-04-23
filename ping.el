(seq material
      (
       list
       ))

(defun sum(lst)
  (if (eq lst nil)
      0
    (add (car lst) 
         (sum (cdr lst)))))

(defun abs(lst)
  (if (eq lst nil)
      nil
    (progn
      (print (car lst))
      (print (system 
              (concat
               (quote curl -d '{"header":{"servicekey":"positioning_service","secretkey":"fec9dfb48b1e2ition_semantics"}, "prod": "map", "longitude": 13533773.27, "latitude" : 3638048.64, "wf" : "c83a3515bf78;81;|94772b96365c;82;|286c07ccd080;85;", "dwnum" : 10 }')
               'SPACE
               (car lst)
               (quote :7778/UserService/GetPlaceSemanticInfo))))
      (abs (cdr lst)))))

(abs material )

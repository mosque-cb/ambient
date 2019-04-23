(defun exactsize (lst)
  (if  (eq lst  nil)
      nil
    (if  (not (eq  (find (car lst) 
                           (quote Content-Length:)) 
                   nil))
        (atoi  (strdup (car lst)
                       (add 2 (find (car lst) (quote : )))
                       (strlen (car lst))))
      (exactsize (cdr lst)))))

(defun getdata(lst)
  (list (helper lst)
        (helper (tail lst 4))))

(defun helper(lst)
  (div (add (mul 256 256 256 (car lst))
            (mul 256 256 (car (cdr lst)))
            (mul 256 (car (cdr (cdr lst))))
            (mul (car (cdr (cdr (cdr lst))))))
       1000000))

(defun strategy(material)
  (if (eq material nil)
      (print 'trying)
    (if (big 100 (strlen material))
        (progn
          (print 'error)
          (print material))
      (progn
        (print (strlen material))
        (if (and (eq 25 (minus  (strlen material)
                                (add 4 (find material (concat 'LINE 'LINE)))))
                 (eq 25 (exactsize (linesplit material))))
            (print (getdata 
                    (tail 
                          (dump_bytes (strdup material
                                                   (add 4 (find material (concat 'LINE 'LINE)))
                                                   (strlen material)))
                          7)))
          (print 'notok))))))

(defun  gene_zero(num)
  (if (eq num 0)
      nil
    (cons 0
          (gene_zero (minus num 1)))))

(defun  deserial(num bytes)
  (if (eq bytes 1)
      (cons num nil)
    (cons (mod num 256)
          (deserial (div num 256)
                    (minus bytes 1)))))

(defun  decision(cid)
  (if (big cid 65536)
      5
    3))

(defun  append(a b)
  (if (eq a nil)
      b
    (cons (car a)
          (append (cdr a)
                  b))))

(defun  concathelper(lst)
  (if (eq lst nil)
      nil
    (append (car lst)
            (concathelper (cdr lst)))))

(defun  formatdata(cid lac mnc mcc)
  (for_bytes
   (concathelper
    (print    (list
               (cons 0 nil)
               (cons 14 nil)
               (gene_zero 14)
               (cons 27 nil)
               (deserial (htonl mnc) 4)
               (deserial (htonl mcc) 4)
               (gene_zero 3)
               (cons (decision cid) nil)
               (gene_zero 2)
               (deserial (htonl cid) 4)
               (deserial (htonl lac) 4)
               (deserial (htonl mnc) 4)
               (deserial (htonl mcc) 4)
               (list 255 255 255 255)
               (gene_zero 4))))))

(defun  handle (socket data)
  (progn
    (send socket (concat (lineconcat (list (quote POST /glm/mmap HTTP/1.1) 
                                           (quote Content-Type: application/binary) 
                                           (quote Host: www.google.com) 
                                           (quote Content-Length: 55) 
                                           (quote Expect: 100-continue) 
                                           (quote Connection: Close)))
                         'LINE
                         'LINE))
    (send socket data)
    (strategy
     (recv socket 1000000))
    (close socket)))

(handle 
 (connect (quote www.google.com))
 (formatdata 10015
             10110
             0
             0))

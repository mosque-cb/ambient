(defun calc(avalue temp height mid low lr)
  (if (eq avalue temp)
      (progn
        (print (concat (storage find:)  temp))
        mid)
    (if (eq low height)
        mid
      (if (big temp avalue)
          (wraplr avalue
                  b
                  (minus mid 1)
                  low
                  lr)
        (wraplr avalue
                b
                height
                (add mid 1)
                lr)))))

(defun landr(avalue b height low mid lr)
  (calc avalue
        (idx b mid)
        height
        mid
        low
        lr))

(defun wraplr(avalue b height low lr)
  (if (big low height)
      lr
    (landr avalue
           b
           height
           low
           (div (add height low)
                2)
           lr)))

(defun wrapbinarys(i a alen b br bl)
  (if (eq alen 
          (add 1 (mul 2 i)))
      (print (storage i map))
    (binarys
     (add i 1)
     a
     alen
     b
     (wraplr (idx a (minus alen i 1))
             b
             br
             bl
             br)
     bl)))

(defun binarys(i a alen b br bl)
  (if (big i (div alen 2))
      (print (storage i big))
    (if (eq alen 
            (mul 2 i))
        (print (storage i equ))
      (wrapbinarys
       i
       a
       alen
       b
       br
       (wraplr (idx a i)
               b
               br
               bl
               bl)))))

(defun intersection(a b)
  (binarys
   0
   a
   (sz a)
   b
   (minus (sz b) 1)
   0))

(defun dispatch(a b)
  (intersection
   (qsort (forstorage a))
   (qsort (forstorage b))))

(defun  gendata(n)
  (if (eq n 0)
      nil
    (cons  (random 1000)
           (gendata (minus n 1)))))

(defun  autotest(n)
  (progn
    (print n)
    (display)
    (print (dispatch (print (gendata 50))
                     (print (gendata 64))))
    (display)
    (autotest (add n 1))))

(autotest 0)

(comment
(print (dispatch (print (list  40 30 50))
                 (print (list 30 40 64 100 200  20  50))))
 )






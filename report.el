(seq material
      '(
        1350112
        5571644
        2451989
        9870846
        15587475
        ))

(defun sum(lst)
  (if (eq lst nil)
      0
    (add (car lst) 
         (sum (cdr lst)))))

(defun abs(lst all material_str)
  (if (eq lst nil)
      nil
    (progn
      (print (concat (itoa (car lst)) 
                     (quote /) 
                     'CURVEL
                     (addconcat material_str)
                     'CURVER
                     (quote =)
                     (div (mul 100 (car lst))
                          all)
                     (quote %)))
      (abs (cdr lst) 
           all
           material_str))))

(abs material 
     (sum material)
     material)

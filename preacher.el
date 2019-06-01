(defun caar (lst)
  (car (car lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun caddr (lst)
  (car (cddr lst)))

(defun printpos (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print  'pos)
      (printlst  (car  n) )
      (printpos (cdr n) ))))

(defun printlst (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print    (car  n) )
      (printlst (cdr n) ))))

(defun evprogn ()
  (if  (eq   (top_ptr  'zendata)  nil)
      (progn
        (remove_ptr   'zendata)
        (add_array  'zendata (remove_ptr   'zencode)))
    (progn
      (add_array  'zencode  (car  (top_ptr 'zendata)))
      (exchange   'zencode)
      (remove_ptr   'zencode)
      (add_array  'zendata  (cdr  (top_ptr 'zendata)))
      (exchange   'zendata)
      (remove_ptr   'zendata)
      (evprogn ))))

(defun  evif ()
  (progn
    (if  (remove_ptr 'zendata)
        (progn
          (add_array  'zencode  (caddr  (top_ptr 'zencode)))
          (exchange   'zencode)
          (remove_ptr   'zencode)
          (wrapeval))
      (progn
        (add_array  'zencode  (cadr (cddr  (top_ptr 'zencode))))
        (exchange   'zencode)
        (remove_ptr   'zencode)
        (wrapeval)))))

(defun  formstruct (lst)
  (if  (eq  lst  nil)
      nil
    (if  (atom lst)
        lst
      (cons  (formstruct (car lst))
             (formstruct (cdr lst))))))

(defun  wrapdefun (lst )
  (progn
    (seq  globaldefun  
          (cons   lst  
                  (geq   globaldefun)))
    (print  'wrapdefun)))

(defun  funp(name global)
  (if  (eq  global  nil)
      0
    (if  (eq  name  (caar global))
        1
      (funp  name  (cdr  global)))))

(defun  findexpr(name global)
  (if  (eq  name  
            (caar  global))
      (cdar  global)
    (findexpr  name  
               (cdr  global))))

(defun  bindvars  (arg value env)
  (cons
   (bindvarshelp arg value)
   env))

(defun  bindvarshelp  (arg value )
  (if  (eq  arg  nil)
      nil
    (cons  ( cons  (car arg)  
                   (cons (car value)
                         nil))
           (bindvarshelp  (cdr  arg) (cdr value)))))

(defun  varfind (arg env)
  (if  (eq  env  nil)
      0
    (if  (eq  arg  
              (car  (car env)))
        (progn
          (progn (seq midx  
                      (cadr (car env))))
          1)
      (varfind arg  
               (cdr env)))))

(defun  varfetch (arg env)
  (if  (eq  arg  
            (car  (car env)))
      (cadr  
       (car env))
    (varfetch arg  
              (cdr env))))

(defun  findvar  (arg  env)
  (if  (varfind   arg    
                  (car env))
      (geq midx)
    (findvar  arg  
              (cdr env))))

(defun   evargslast()
  (progn
    (if  (eq  (top_ptr 'zencode)
              'evprogn)
        (progn
          (remove_ptr 'zencode)
          (add_array  'zencode  nil)
          (evprogn)
          (popjreturn))
      (if  (eq  (top_ptr 'zencode)
                'printpos)
          (progn
            (remove_ptr 'zencode)
            (add_array  'zencode
                       (printpos  (remove_ptr 'zendata)))
            (add_array 'zendata (remove_ptr  'zencode))
            (popjreturn))
        (if  (eq  (top_ptr 'zencode)
                  'printlst)
            (progn
              (remove_ptr 'zencode)
              (progn  (add_array  'zencode
                                 (printlst  (remove_ptr  'zendata))))
              (progn  (add_array 'zendata  (remove_ptr 'zencode)))
              (popjreturn))
          (if  (primitivep  (top_ptr 'zencode))
              (progn
                (progn  (seq midx
                             (primitive (remove_ptr 'zencode)
                                        (remove_ptr  'zendata))))
                (progn  (add_array 'zendata  midx))
                (popjreturn))
            (sapply)
            ))))))

(defun  sapply ()
  (progn    
    (progn  (add_array 'zencode 
                      (findexpr (top_ptr 'zencode)
                                (geq globaldefun))))
    (progn  (exchange  'zencode))
    (remove_ptr  'zencode)
    (progn  (seq  'env 
                  (bindvars         (car (top_ptr 'zencode))
                                    (remove_ptr  'zendata)
                                    (geq  'env))))
    (progn  (add_array 'zencode  (cadr  (top_ptr 'zencode))))
    (progn  (exchange  'zencode))
    (remove_ptr  'zencode)
    (wrapeval)))

(defun   append (a b)
  (if (eq a nil)
      b
    (cons (car a)
          (append (cdr a)
                  b))))
  
(defun   combi (a b addr)
  (add_array addr
           (append b a)))

(defun   evargscombi ()
  (progn
    (progn  (combi  (remove_ptr 'zendata)
                    (remove_ptr 'zendata)
                    'zendata))
    (progn  (seq   'env 
                   (remove_ptr  'zencode)))
    (evargs)))

(defun   evargs()
  (progn
    (if  (eq (top_ptr 'zencode)  nil)
        (progn
          (remove_ptr  'zencode)
          (evargslast))
      (progn
        (progn   (add_array   'zendata  
                             (car (top_ptr 'zencode))))
        (progn   (add_array   'zencode  
                             (cdr (top_ptr 'zencode))))
        (progn  (exchange  'zencode))
        (remove_ptr  'zencode)

        (progn   (add_array   'zencode  
                             (geq 'env)))

        (progn   (add_array   'zencode  
                             'evargscombi))
        (progn   (add_array   'zencode  
                             (remove_ptr  'zendata)))
        (wrapeval)))))

(defun  popjreturn ()
  (progn
    (if  (eq (top_ptr 'zencode) nil)
        (remove_ptr  'zendata)
      (funcall  (remove_ptr 'zencode)))))

(defun   wrapeval ()
  (progn
    (if  (digitp  (top_ptr 'zencode ))
        (progn
          (progn  (add_array  'zendata
                             (remove_ptr 'zencode)))
          (popjreturn))
      (if  (eq  (top_ptr 'zencode) nil)
          (progn
            (progn  (add_array  'zendata  (remove_ptr 'zencode)))
            (popjreturn))
        (if  (charp  (top_ptr 'zencode))
            (progn
              (progn  (add_array  'zendata
                                 (findvar (remove_ptr 'zencode)
                                          (geq  'env))))
              (popjreturn))
          (if  (eq (car (top_ptr 'zencode))  'quote)
              (progn
                (progn  (add_array  'zendata
                                   (cadr  (remove_ptr 'zencode))))
                (popjreturn))
            (if  (eq (car (top_ptr 'zencode))  'if)
                (progn
                  (progn   (add_array   'zendata
                                       (cadr (top_ptr 'zencode))))
                  (progn   (add_array   'zencode  
                                       (top_ptr 'zencode)))
                  (progn   (exchange    'zencode))
                  (remove_ptr  'zencode)
                  (progn   (add_array   'zencode  
                                       'evif))
                  (progn   (add_array   'zencode  (remove_ptr  'zendata)))
                  (wrapeval))
              (if  (eq (car (top_ptr 'zencode))  'defun)
                  (wrapdefun  (cdr (remove_ptr 'zencode )))
                (if  (eq (car (top_ptr 'zencode))  'progn)
                    (progn
                      (progn  (add_array   'zendata  (remove_ptr  'zencode)))
                      (progn   (add_array   'zencode  
                                           'evprogn))
                      (progn   (add_array   'zencode  
                                           (cdr  (remove_ptr  'zendata))))
                      (progn  (add_array   'zendata
                                          nil))
                      (evargs))
                  (progn
                    (progn   (add_array   'zendata
                                         (cdr  (top_ptr 'zencode))))
                    (progn   (add_array   'zencode  
                                         (car  (top_ptr 'zencode))))
                    (progn   (exchange    'zencode))
                    (remove_ptr  'zencode)
                    (progn   (add_array   'zencode  
                                         (remove_ptr  'zendata)))
                    (progn  (add_array  'zendata
                                       nil))
                    (evargs)))))))))))

(defun  generand  (count range)
  (if  (eq  count  0)
      nil
    (cons    (random  range)
             (generand  (minus  count  1)
                        range))))

(defun once  ( )
  (progn 
    (display )
    (seq  'env     nil)
    (seq  'midx  nil)
    (seq  'globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (seq  'env     nil)
    (seq  'midx  nil)
    (seq  'globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (quote times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (progn   (add_array  'zencode  (car  lst)))
      (print   (wrapeval))
      (main (cdr lst)  ))))

(define  basicdefun   '( 
                        (defun  generand  (count range)
                          (if  (eq  count  0)
                              nil
                            (cons    (random  range)
                                     (generand  (minus  count  1)
                                                range))))

                        (defun  value_x  (position)
                          (car  position))

                        (defun  value_y  (position)
                          (car (cdr  position)))

                        (defun  form_pos  (  x  y)
                          (cons  x  (cons  y 
                                           nil)))

                        (defun  add_x_pos (position)
                          (form_pos (add  (value_x position)  
                                          1)
                                    (value_y  position)))

                        (defun  minus_x_pos (position)
                          (form_pos (minus  (value_x position)  
                                            1)
                                    (value_y  position)))

                        (defun  add_y_pos (position)
                          (form_pos (value_x position) 
                                    (add (value_y  position)
                                         1)))

                        (defun  minus_y_pos (position)
                          (form_pos (value_x position) 
                                    (minus (value_y  position)
                                           1)))

                        (defun  up_y (n path)
                          (if (eq  n  0)
                              path
                            (up_y  (minus  n 
                                           1)
                                   (cons  (add_y_pos (car path))
                                          path))))

                        (defun  down_y (n path)
                          (if (eq  n  0)
                              path
                            (down_y  (minus  n 
                                             1)
                                     (cons  (minus_y_pos (car path))
                                            path))))

                        (defun  up_x (n path)
                          (if (eq  n  0)
                              path
                            (up_x  (minus  n  
                                           1)
                                   (cons  (add_x_pos (car path))
                                          path))))

                        (defun  down_x (n path)
                          (if (eq  n  0)
                              path
                            (down_x  (minus  n 
                                             1)
                                     (cons  (minus_x_pos (car path))
                                            path))))

                        (defun  wrap_helpery  (path eggs)
                          (helpery  (value_y (car  path))
                                    (value_y  (car eggs))
                                    path))

                        (defun   helpery  (fro  toy path )
                          (if  (big  fro  toy)                
                              (down_y  (minus fro toy)
                                       path)
                            (up_y  (minus toy fro ) 
                                   path)))

                        (defun move_left (from  )
                          (down_x  (minus (value_x (car from)) 
                                          1)
                                   from ))

                        (defun move_right (from )
                          (up_x  (minus 10
                                        (value_x (car from)) )
                                 from ))

                        (defun  collision (  path eggs direct  result)
                          (strategy           path
                                              (cdr eggs)
                                              direct
                                              (cons  (car path) result)))

                        (defun   wrap_helperx  ( path eggs  direct result)
                          (helperx  (value_x  (car  path))
                                    (value_x  (car  eggs))
                                    path
                                    eggs
                                    direct
                                    result))

                        (defun   helperx  (fromx  tox path eggs  direct result)
                          (if (eq  fromx  tox)
                              (collision   path  
                                           eggs 
                                           direct
                                           result)
                            (if  (big  fromx  tox)                
                                (collision	      (down_x  (minus fromx tox) 
                                                           path) 
                                                  eggs 
                                                  direct
                                                  result)
                              (collision	    (up_x  (minus tox fromx ) 
                                                       path )
                                                eggs
                                                direct
                                                result))))

                        (defun  form_eggs (xpath ypath)
                          (if  (eq  xpath nil)
                              nil
                            (cons  (form_pos (car xpath)
                                             (car ypath))
                                   (form_eggs  (cdr xpath)
                                               (cdr  ypath)))))

                        (defun  wrap_strategy (eggs)
                          (progn
                            (print  'eggs)
                            (printpos eggs)
                            (print 'path)
                            (printpos  (strategy  (cons (car eggs) 
                                                        nil)
                                                  (cdr eggs) 
                                                  0
                                                  (cons (car eggs) 
                                                        nil)))))

                        (defun  snake  ()
                          (wrap_strategy (form_eggs 
                                          (generand  10  10)
                                          (generand  10  10)
                                          )))

                        (defun  strategy (path  eggs direct result)
                          (if  (eq  eggs  nil)
                              result
                            (if  (eq  (value_y  (car path))        
                                      (value_y  (car eggs)))
                                (if  (big  (value_x  (car path))        
                                           (value_x  (car eggs)))
                                    (if  (eq  direct  0)
                                        (collision		(down_x  (minus   (value_x  (car path))
                                                                          (value_x  (car eggs)))
                                                                 path)
                                                        eggs
                                                        direct
                                                        result)
                                      (progn
                                        (print 'back)
                                        (if  (eq  (value_y  (car path))  10)
                                            (strategy  (down_y  1      (move_right  path))
                                                       eggs
                                                       (minus  1 direct)
                                                       result)
                                          (strategy  (up_y  1      (move_right  path))
                                                     eggs
                                                     (minus  1 direct)
                                                     result))))
                                  (if  (eq  direct  0)
                                      (progn
                                        (print 'back)
                                        (if  (eq  (value_y  (car path))  10)
                                            (strategy  (down_y  1      (move_left  path))
                                                       eggs
                                                       (minus  1 direct)
                                                       result)
                                          (strategy  (up_y  1      (move_left  path))
                                                     eggs
                                                     (minus  1 direct)
                                                     result)))
                                    (collision		(up_x    (minus   (value_x  (car eggs))
                                                                      (value_x  (car path)))
                                                             path)
                                                    eggs
                                                    direct
                                                    result)
                                    ))
                              (if  (eq  direct  0)
                                  (wrap_helperx  (wrap_helpery      (move_left  path)
                                                                    eggs)
                                                 eggs
                                                 (minus  1 direct)
                                                 result)
                                (wrap_helperx  (wrap_helpery       (move_right  path)
                                                                   eggs)
                                               eggs
                                               (minus  1 direct)
                                               result)))))
                        ))

(define  basicapply    '( 
                         (snake)
                         ))

(define  basicshow    'hello)
(print   'initobject)
(once)
(autotest 0 )

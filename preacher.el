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
  (if  (eq   (top  zendata)  nil)
      (progn
        (eject   zendata)
        (press  zendata (eject   zencode)))
    (progn
      (press  zencode  (car  (top zendata)))
      (exchange   zencode)
      (eject   zencode)
      (press  zendata  (cdr  (top zendata)))
      (exchange   zendata)
      (eject   zendata)
      (evprogn ))))

(defun  evif ()
  (progn
    (if  (eject zendata)
        (progn
          (press  zencode  (caddr  (top zencode)))
          (exchange   zencode)
          (eject   zencode)
          (wrapeval))
      (progn
        (press  zencode  (cadr (cddr  (top zencode))))
        (exchange   zencode)
        (eject   zencode)
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
    (setq  globaldefun  
           (cons   lst  
                   (whole   globaldefun)))
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
          (progn (setq midx  
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
      (whole midx)
    (findvar  arg  
              (cdr env))))

(defun   evargslast()
  (progn
    (if  (eq  (top zencode)
              'evprogn)
        (progn
          (eject zencode)
          (press  zencode  nil)
          (evprogn)
          (popjreturn))
      (if  (eq  (top zencode)
                'printpos)
          (progn
            (eject zencode)
            (press  zencode
                    (printpos  (eject zendata)))
            (press zendata (eject  zencode))
            (popjreturn))
        (if  (eq  (top zencode)
                  'printlst)
            (progn
              (eject zencode)
              (progn  (press  zencode
                              (printlst  (eject  zendata))))
              (progn  (press zendata  (eject zencode)))
              (popjreturn))
          (if  (primitivep  (top zencode))
              (progn
                (progn  (setq midx
                              (primitive (eject zencode)
                                         (eject  zendata))))
                (progn  (press zendata  midx))
                (popjreturn))
            (sapply)
            ))))))

(defun  sapply ()
  (progn    
    (progn  (press zencode 
                   (findexpr (top zencode)
                             (whole globaldefun))))
    (progn  (exchange  zencode))
    (eject  zencode)
    (progn  (setq  env 
                   (bindvars         (car (top zencode))
                                     (eject  zendata)
                                     (whole  env))))
    (progn  (press zencode  (cadr  (top zencode))))
    (progn  (exchange  zencode))
    (eject  zencode)
    (wrapeval)))

(defun   evargscombi ()
  (progn
    (progn  (combi  zendata))
    (progn  (setq   env 
                    (eject  zencode)))
    (evargs)))

(defun   evargs()
  (progn
    (if  (eq (top zencode)  nil)
        (progn
          (eject  zencode)
          (evargslast))
      (progn
        (progn   (press   zendata  
                          (car (top zencode))))
        (progn   (press   zencode  
                          (cdr (top zencode))))
        (progn  (exchange  zencode))
        (eject  zencode)

        (progn   (press   zencode  
                          (whole env)))

        (progn   (press   zencode  
                          'evargscombi))
        (progn   (press   zencode  
                          (eject  zendata)))
        (wrapeval)))))

(defun  popjreturn ()
  (progn
    (if  (eq (top zencode) nil)
        (eject  zendata)
      (funcall  (eject zencode)))))

(defun   wrapeval ()
  (progn
    (if  (digitp  (top zencode ))
        (progn
          (progn  (press  zendata
                          (eject zencode)))
          (popjreturn))
      (if  (eq  (top zencode) nil)
          (progn
            (progn  (press  zendata  (eject zencode)))
            (popjreturn))
        (if  (charp  (top zencode))
            (progn
              (progn  (press  zendata
                              (findvar (eject zencode)
                                       (whole  env))))
              (popjreturn))
          (if  (eq (car (top zencode))  'quote)
              (progn
                (progn  (press  zendata
                                (cadr  (eject zencode))))
                (popjreturn))
            (if  (eq (car (top zencode))  'if)
                (progn
                  (progn   (press   zendata
                                    (cadr (top zencode))))
                  (progn   (press   zencode  
                                    (top zencode)))
                  (progn   (exchange    zencode))
                  (eject  zencode)
                  (progn   (press   zencode  
                                    'evif))
                  (progn   (press   zencode  (eject  zendata)))
                  (wrapeval))
              (if  (eq (car (top zencode))  'defun)
                  (wrapdefun  (cdr (eject zencode )))
                (if  (eq (car (top zencode))  'progn)
                    (progn
                      (progn  (press  zendata  (eject  zencode)))
                      (progn   (press   zencode  
                                        'evprogn))
                      (progn   (press   zencode  
                                        (cdr  (eject  zendata))))
                      (progn  (press   zendata
                                       nil))
                      (evargs))
                  (progn
                    (progn   (press   zendata
                                      (cdr  (top zencode))))
                    (progn   (press   zencode  
                                      (car  (top zencode))))
                    (progn   (exchange    zencode))
                    (eject  zencode)
                    (progn   (press   zencode  
                                      (eject  zendata)))
                    (progn  (press  zendata
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
    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (storage times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (progn   (press  zencode  (car  lst)))
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
